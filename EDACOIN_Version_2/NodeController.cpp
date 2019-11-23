#include "NodeController.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

void NodeController::cycle(void) {
	for (int i = 0; i < m.getFullNodesSize(); i++) {
		FullNodeInfo& info = m.getFullNode(i);
		FullNode* n = info.fullNode;

		//Client
		if (n->isPerformingFetch()) {
			n->performFetch();
		}
		else {
			if (n->getHttpMessage() == POST) {
				/*json response(n->getResponse());
				if (response["result"] == true) {
					cout << "No error in response" << endl;
					//Yay for me
				}
				else {
					cout << "Error in response";
				}*/
			}
			else if (n->getHttpMessage() == GET) {
				json response(n->getResponse());

				//Received Block Header, do something
			}
		}

		//Server
		n->cycleConnections();

		//Full Node GUI
		ImGui::Begin(("Full Node " + n->getNodeID()).c_str());

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

		if (ImGui::Button("Boton de prueba")) {
		}

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
			ImGui::End();
		}

		ImGui::End();
	}
	for (int i = 0; i < m.getSPVNodesSize(); i++) {
		SPVNodeInfo info = m.getSPVNode(i);
		SPVNode* n = info.spvNode;

		//Client
		if (n->isPerformingFetch()) {
			n->performFetch();
		}
		else {
			if (n->getHttpMessage() == POST) {
				json response(n->getResponse());
				if (response["result"] == true) {
					//Yay for me
				}
			}
			else if (n->getHttpMessage() == GET) {
				json response(n->getResponse());

				//Received Block Header, do something
			}
		}

		//SPV Node GUI
		ImGui::Begin(("SPV Node " + n->getNodeID()).c_str());

		ImGui::Text(("ID: " + n->getNodeID()).c_str());
		ImGui::Text(("IP: " + n->getNodeIP()).c_str());
		ImGui::Text(("Puerto: " + to_string(n->getNodePort())).c_str());
		ImGui::Separator();
		ImGui::Text("Conectarse con otro nodo");
		char id[50];
		sprintf(id, info.neighbourID.c_str());
		ImGui::InputText("Ingresar ID", id, sizeof(id));
		info.neighbourID = id;

		if (ImGui::Button("Boton de prueba")) {
			n->sendFilter(id);
		}
		if (ImGui::Button("Transferencia")) {
			//n->httpGet(id, "/test.html");
			char NeighID[64];
			float amount = 0;
			ImGui::Begin("Transfer");
			ImGui::Text("ID del vecino", NeighID);
			ImGui::InputFloat("Monto", &amount, 0.1f, 0.5f);
			if (ImGui::Button("Aceptar")) {
				/*realizar transaccion*/
			}
			ImGui::End();

		}
		if (ImGui::Button("Ver Vecinos")) {
			char NeighID1[64] = "Vecino 1";
			char NeighID2[64] = "Vecino 2";
			ImGui::Text("Id", NeighID1);
			ImGui::Text("Id", NeighID2);
			/*
			Cambiar vecinos
			*/
		}

	}
}