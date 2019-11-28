#include "NodeController.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <fstream>

NodeController::NodeController(LocalNodes& mod) : m(mod) {}

void NodeController::cycle(void) {
	static string newip = "";
	static int newport = 0;
	static string newid = "";

	//GUI
	ImGui::Begin("Node Controller");

	char ip[50];
	int port = newport;
	char id[50];
	sprintf(ip, newip.c_str());
	sprintf(id, newid.c_str());
	ImGui::InputText("Ingresar IP", ip, sizeof(ip));
	ImGui::InputInt("Ingresar Puerto", &port, 0.1f, 0.5f);
	ImGui::InputText("Ingresar ID", id, sizeof(id));
	newip = ip;
	newport = port;
	newid = id;

	if (ImGui::Button("Crear un Full Service Node")) m.addFullNode({ ip, port }, id);
	ImGui::SameLine();
	if (ImGui::Button("Crear un SPV Node")) m.addSPVNode({ ip, port }, id);

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Columns(2);

	ImGui::BeginChild("LocalNodeList", ImVec2(ImGui::GetColumnWidth() - 10, 400));

	ImGui::Text("Local Full Service Nodes");
	ImGui::Separator();

	bool nodeSelected = false;
	int nodeIndex = -1;
	if(m.isSelectedNodeFull()) nodeIndex = m.getSelectedNodeIndex();
	for (int i = 0; i < m.getFullNodesSize(); i++) {
		string nodeid = m.getFullNode(i).fullNode->getNodeID();
		nodeSelected = ImGui::MenuItem((" " + nodeid).c_str(), nullptr, (nodeIndex == i) ? true : false);
		if (nodeSelected) m.setSelectedNode(i, true);
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Text("Local Simplified Payment Verification Nodes");
	ImGui::Separator();
	nodeSelected = false;
	nodeIndex = -1;
	if (m.isSelectedNodeFull() == false) nodeIndex = m.getSelectedNodeIndex();
	for (int i = 0; i < m.getSPVNodesSize(); i++) {
		string nodeid = m.getSPVNode(i).spvNode->getNodeID();
		nodeSelected = ImGui::MenuItem((" " + nodeid).c_str(), nullptr, (nodeIndex == i) ? true : false);
		if (nodeSelected) m.setSelectedNode(i, false);
	}
	ImGui::EndChild();	//Lista de Nodos

	ImGui::NextColumn();
	ImGui::BeginChild("Opened Node", ImVec2(ImGui::GetColumnWidth() - 10, 400));

	if (m.isAnyNodeSelected()) {
		if (m.isSelectedNodeFull()) {
			showFullNodeGUI(m.getSelectedFullNode());
		}
		else {
			showSPVNodeGUI(m.getSelectedSPVNode());
		}
	}

	ImGui::EndChild();	//Opened Node
	ImGui::Columns();
	ImGui::Separator();
	ImGui::End();

	//Server y Client

	for (int i = 0; i < m.getFullNodesSize(); i++) {
		FullNodeInfo& info = m.getFullNode(i);
		FullNode* n = info.fullNode;

		//Client
		if (n->isPerformingFetch()) {
			n->performFetch();
		}
		else {
			n->handleResponse();
		}

		//p2p fsm
		n->p2pFSM();

		//Server
		n->cycleConnections();
	}

	for (int i = 0; i < m.getSPVNodesSize(); i++) {
		/*SPVNodeInfo& info = m.getSPVNode(i);
		SPVNode* n = info.spvNode;

		//Client
		if (n->isPerformingFetch()) {
			n->performFetch();
		}
		else {
			if (n->getHttpMethod() == "POST") {
				json response(n->getResponse());
				if (response["result"] == true) {
					cout << "No error in response" << endl;
					//Yay for me
				}
				else {
					cout << "Error in response";
				}
			}
			else if (n->getHttpMethod() == "GET") {
				json response(n->getResponse());

				//Received Block Header, do something
			}
		}

		//Server
		n->cycleConnections();*/
	}
}

void NodeController::showFullNodeGUI(FullNodeInfo& info) {
	FullNode* n = info.fullNode;
	
	if(ImGui::Button("Prueba")){
		n->httpGet("127.0.0.1", 1001, "/test/");
	}
	
	//Full Node GUI
	ImGui::Text("Informacion del Nodo");
	ImGui::Separator();
	ImGui::Text(("ID: " + n->getNodeID()).c_str());
	ImGui::Text(("IP: " + n->getNodeIP()).c_str());
	ImGui::Text(("Puerto: " + to_string(n->getNodePort())).c_str());
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Text("Conectarse con otro nodo");
	ImGui::Separator();
	char id[50];
	sprintf(id, info.neighbourID.c_str());
	ImGui::InputText("Ingresar ID de nodo vecino", id, sizeof(id));
	info.neighbourID = id;
	ImGui::NewLine();

	if (ImGui::Button("Grafo")) {
		if (info.graph == NULL) {
			info.graph = info.graphNeighbours();
			if (info.graph != NULL) { info.graphi = true; }
		}
	}

	if (ImGui::CollapsingHeader("Realizar Transferencia")) {
		int amount = info.amount;
		ImGui::InputInt("Monto", &amount, 0.1f, 0.5f);
		info.amount = amount;
		char receiver[50];
		sprintf(receiver, info.receiver.c_str());
		ImGui::InputText("Receptor", receiver, sizeof(receiver));
		info.receiver = receiver;
		if (ImGui::Button("Transferir")) {
			vector<vinType> vin;
			vin.push_back({ "blockid-dummy", "txid-dummy" });
			vector<voutType> vout;
			vout.push_back({ info.receiver, amount });
			Transaction tx = { "txid-dummy", vin.size(), vin, vout.size(), vout };
			n->sendTx(id, tx);
		}
		ImGui::NewLine();
	}
	
	if (ImGui::CollapsingHeader("Agregar Vecino")) {
		char newip[50];
		int newport = info.newport;
		char newid[50];
		sprintf(newip, info.newip.c_str());
		sprintf(newid, info.newid.c_str());
		ImGui::InputText("IP del nuevo Vecino", newip, sizeof(newip));
		ImGui::InputInt("Puerto del nuevo Vecino", &newport, 0.1f, 0.5f);
		ImGui::InputText("ID del nuevo Vecino", newid, sizeof(newid));
		info.newip = newip;
		info.newport = newport;
		info.newid = newid;
		if (ImGui::Button("Agregar vecino")) {
			n->appendNeighbourNode(newid, { newip, newport });
		}
		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Ver Bloques")) {

		for (int i = 0; i < n->getBlockchain().getBlockchainSize(); i++) {
			bool selectedBlock = n->getBlockchain().getBlockSelected(i);
			ImGui::Checkbox(("##Bloque " + to_string(i) + n->getNodeID()).c_str(), &selectedBlock);
			n->getBlockchain().setBlockSelected(i, selectedBlock);
			n->getBlockchain().setBlockShowingInfo(i, true);
			n->getBlockchain().setBlockShowingInfo(i, false);
		}
		if (ImGui::Button("Seleccionar todos")) {
			for (int i = 0; i < n->getBlockchain().getBlockchainSize(); i++) {
				n->getBlockchain().setBlockSelected(i, true);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Quitar seleccion")) {
			for (int i = 0; i < n->getBlockchain().getBlockchainSize(); i++) {
				n->getBlockchain().setBlockSelected(i, false);
			}
		}
		if (ImGui::Button("Abrir Bloques")) {
			for (int i = 0; i < n->getBlockchain().getBlockchainSize(); i++) {
				if (n->getBlockchain().getBlockSelected(i)) {
					n->getBlockchain().setBlockOpened(i, true);
				}
				n->getBlockchain().setBlockSelected(i, false);
			}
		}

		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Ver Vecinos")) {
		map<string, SocketType> neighbours = n->getNeighbours();
		map<string, SocketType>::iterator it;
		for (it = neighbours.begin(); it != neighbours.end(); ++it)
		{
			ImGui::Text((it->first).c_str());
		}

		ImGui::NewLine();
	}
	if (info.graphi) {
		ImGui::Begin("Graph");
		ImGui::Image(info.graph, ImVec2(al_get_bitmap_width(info.graph), al_get_bitmap_height(info.graph)));
		ImGui::End();
	}
}
void NodeController::showSPVNodeGUI(SPVNodeInfo& info) {
	SPVNode* n = info.spvNode;

	//SPV Node GUI
	ImGui::Text("Informacion del Nodo");
	ImGui::Separator();
	ImGui::Text(("ID: " + n->getNodeID()).c_str());
	ImGui::Text(("IP: " + n->getNodeIP()).c_str());
	ImGui::Text(("Puerto: " + to_string(n->getNodePort())).c_str());
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Text("Conectarse con otro nodo");
	ImGui::Separator();
	char id[50];
	sprintf(id, info.neighbourID.c_str());
	ImGui::InputText("Ingresar ID de nodo vecino", id, sizeof(id));
	info.neighbourID = id;
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Realizar Transferencia")) {
		int amount = info.amount;
		ImGui::InputInt("Monto", &amount, 0.1f, 0.5f);
		info.amount = amount;
		char receiver[50];
		sprintf(receiver, info.receiver.c_str());
		ImGui::InputText("Receptor", receiver, sizeof(receiver));
		info.receiver = receiver;
		if (ImGui::Button("Transferir")) {
			vector<vinType> vin;
			vin.push_back({ "blockid-dummy", "txid-dummy" });
			vector<voutType> vout;
			vout.push_back({ info.receiver, amount });
			Transaction tx = { "txid-dummy", vin.size(), vin, vout.size(), vout };
			n->sendTx(id, tx);
		}
		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Modificar Vecino")) {
		bool selected = false;
		ImGui::Text("Vecino a modificar: ");
		map<string, SocketType> neighbours = n->getNeighbours();
		map<string, SocketType>::iterator it;
		for (it = neighbours.begin(); it != neighbours.end(); ++it)
		{
			 selected = (info.oldNeighbourID == it->first) ? true : false;
			ImGui::Checkbox((it->first).c_str(), &selected);
			info.oldNeighbourID = (selected) ? it->first : info.oldNeighbourID;
		}
		ImGui::NewLine();
		char newip[50];
		int newport = info.newport;
		char newid[50];
		sprintf(newip, info.newip.c_str());
		sprintf(newid, info.newid.c_str());
		ImGui::InputText("IP del nuevo Vecino", newip, sizeof(newip));
		ImGui::InputInt("Puerto del nuevo Vecino", &newport, 0.1f, 0.5f);
		ImGui::InputText("ID del nuevo Vecino", newid, sizeof(newid));
		info.newip = newip;
		info.newport = newport;
		info.newid = newid;
		if (ImGui::Button("Modificar vecino")) {
			if (selected) {
				n->removeNeighbourNode(info.oldNeighbourID);
				n->appendNeighbourNode(newid, { newip, newport });
			}
		}
		ImGui::NewLine();
	}

	//Algo de verificar el MerkleBlock
}