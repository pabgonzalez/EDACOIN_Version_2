#include "NodeController.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <fstream>

NodeController::NodeController(LocalNodes& mod) : m(mod) {
	
}

void NodeController::cycle(void) {
	static string newip = "";
	static string newport = "";
	static string newid = "";


	//GUI
	ImGui::Begin("Node Controller");

	char ip[50];
	char port[50];
	char id[50];
	sprintf(ip, newip.c_str());
	sprintf(port, newport.c_str());
	sprintf(id, newid.c_str());
	ImGui::InputText("Ingresar IP", ip, sizeof(ip));
	ImGui::InputText("Ingresar Puerto", port, sizeof(port));
	ImGui::InputText("Ingresar ID", id, sizeof(id));
	newip = ip;
	newport = port;
	newid = id;

	if (ImGui::Button("Crear un Full Service Node")) {
		map<string, SocketType> neighbours;
		//Leer un archivo con la informacion de los vecinos

		/*for (unsigned int i = 0; i < m.getFullNodesSize(); i++) {
			FullNode* n = m.getFullNode(i).fullNode;
			neighbours[n->getNodeID()] = { n->getNodeIP(), n->getNodePort() };
		}*/
		m.addFullNode({ ip, stoi(port) }, id, neighbours);
	}
	if (ImGui::Button("Crear un SPV Node")) {
		map<string, SocketType> neighbours;
		//Leer un archivo con la informacion de los vecinos

		/*for (unsigned int i = 0; i<2 && i < m.getFullNodesSize(); i++) {
			FullNode* n = m.getFullNode(i).fullNode;
			neighbours[n->getNodeID()] = { n->getNodeIP(), n->getNodePort() };
		}*/
		m.addSPVNode({ ip, stoi(port) }, id, neighbours);
	}

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Columns(2);

	ImGui::BeginChild("LocalNodeList", ImVec2(ImGui::GetColumnWidth() - 10, 300));

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
	ImGui::BeginChild("Opened Node", ImVec2(ImGui::GetColumnWidth() - 10, 300));

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
			if (n->getHttpMethod() == "POST") {
				/*json response(n->getResponse());
				if (response["result"] == true) {
					cout << "No error in response" << endl;
					//Yay for me
				}
				else {
					cout << "Error in response";
				}*/
			}
			else if (n->getHttpMethod() == "GET") {
				json response(n->getResponse());

				//Received Block Header, do something
			}
		}

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

	//Full Node GUI
	ImGui::Text(("ID: " + n->getNodeID()).c_str());
	ImGui::Text(("IP: " + n->getNodeIP()).c_str());
	ImGui::Text(("Puerto: " + to_string(n->getNodePort())).c_str());
	ImGui::Separator();
	ImGui::Text("Conectarse con otro nodo");
	char id[50];
	sprintf(id, info.neighbourID.c_str());
	ImGui::InputText("Ingresar ID", id, sizeof(id));
	info.neighbourID = id;
	ImGui::Separator();

	int amount = info.amount;
	ImGui::InputInt("Monto", &amount, 0.1f, 0.5f);
	info.amount = amount;
	ImGui::SameLine();
	if (ImGui::Button("Transferir")) {
		vector<vinType> vin;
		vin.push_back({ "blockid-dummy", "txid-dummy" });
		vector<voutType> vout;
		vout.push_back({ id, amount });
		Transaction tx = { "txid-dummy", vin.size(), vin, vout.size(), vout };
		n->sendTx(id, tx);
	}
	ImGui::Separator();
	if (ImGui::Button("Ver Bloques")) {
		ImGui::Begin("Bloques");
		for (int i = 0; i < n->getBlockchain().getBlockchainSize(); i++) {
			bool selectedBlock = n->getBlockchain().getBlockSelected(i);
			ImGui::Checkbox(("##Bloque " + to_string(i)).c_str(), &selectedBlock);
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

		ImGui::End();
	}
	if (ImGui::Button("Agregar vecino")) {
		char NeighID[64];
		char NeighPORT[64];
		char NeighIP[64];
		ImGui::Begin("New Neighbour");
		ImGui::Text("ID del vecino", NeighID);
		ImGui::Text("IP del vecino", NeighIP);
		ImGui::Text("PORT del vecino", NeighPORT);
		if (ImGui::Button("Aceptar")) {
			/*agregar vecino*/
		}
	}
}

void NodeController::showSPVNodeGUI(SPVNodeInfo& info) {
}