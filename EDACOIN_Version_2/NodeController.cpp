#include "NodeController.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

void NodeController::cycle(void) {
	for (unsigned int i = 0; i < fullNodes.size(); i++) {
		FullNode* n = fullNodes[i];

		//Client
		if (n->isPerformingFetch())
			n->performFetch();

		//Server
		n->acceptConnection();
		n->readRequest();
		n->writeResponse();

		//Full Node GUI
		ImGui::Begin(("Full Node " + n->getNodeID()).c_str());

		ImGui::Text(("ID: " + n->getNodeID()).c_str());
		ImGui::Text(("IP: " + n->getNodeIP()).c_str());
		ImGui::Text(("Puerto: " + to_string(n->getNodePort())).c_str());
		ImGui::Separator();
		ImGui::Text("Conectarse con otro nodo");
		char id[50];
		sprintf(id, n->getNeighbourID().c_str());
		ImGui::InputText("Ingresar ID", id, sizeof(id));
		n->setNeighbourID(id);

		if (ImGui::Button("Boton de prueba")) {
			n->requestAddress(id, "/test.html");
		}
		
		ImGui::End();
	}
}