#include "NodeController.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

void NodeController::cycle(void) {
	for (int i = 0; i < fullNodes.size(); i++) {
		//Server
		fullNodes[i]->acceptConnection();
		fullNodes[i]->readRequest();
		fullNodes[i]->writeResponse();

		//Gui
		ImGui::Begin(("Node " + to_string(i)).c_str());

		char ip[25] = {};
		char port[25] = {};
		ImGui::InputText("Ingresar IP", ip, sizeof(ip));
		ImGui::InputText("Ingresar Puerto", port, sizeof(port));

		ImGui::Button("Conectar");
		
		ImGui::End();
	}
}