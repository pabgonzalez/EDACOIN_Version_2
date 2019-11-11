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
		if (n->isPerformingFetch())
			n->performFetch();

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

		if (ImGui::Button("Boton de prueba")) {
			n->httpGet(id, "/test.html");
		}
		
		ImGui::End();
	}

	for (int i = 0; i < m.getSPVNodesSize(); i++) {
		SPVNodeInfo info = m.getSPVNode(i);
		SPVNode* n = info.spvNode;

		//Client
		if (n->isPerformingFetch())
			n->performFetch();

		//Full Node GUI
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
			n->httpGet(id, "/test.html");
		}

		ImGui::End();
	}

}