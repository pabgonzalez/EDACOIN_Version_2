#include <iostream>
#include <string>
#include <fstream>
#include "BlockViewerController.h"

using namespace std;

BlockViewerController::BlockViewerController(ALLEGRO_DISPLAY** disp, BlockchainModel& modelo) : Controller(modelo) {
	
	display = (*disp);
	eventQueue = al_create_event_queue();
	if (eventQueue != NULL)
	{
		al_register_event_source(eventQueue, al_get_keyboard_event_source());
		al_register_event_source(eventQueue, al_get_mouse_event_source());
		al_register_event_source(eventQueue, al_get_display_event_source(display));
	}
}

BlockViewerController::~BlockViewerController()
{
	al_unregister_event_source(eventQueue, al_get_display_event_source(display));
	al_destroy_event_queue(eventQueue);
}

void BlockViewerController::update(void* objeto) {
	//printf("Pantalla actualizada\n");
}

void BlockViewerController::run() {
	while (ev.type != FIN_IMGUI) {
		cycle();
	}
}

void BlockViewerController::cycle() {
	if (al_get_next_event(eventQueue, &ev)) {
		queueEmpty = false;
		dispatch();
	}
	else {
		queueEmpty = true;
		dispatch();
	}
}

void BlockViewerController::getEvent() {
	if (!al_get_next_event(eventQueue, &ev))
		ev.type = NO_EVENT;
}

void BlockViewerController::dispatch() {
	static char oldpath[256] = {};
	static char newpath[256] = {};
	static vector<bool> selectedBlocks;
	static vector<bool> openBlocks;

	if (!queueEmpty){
		ImGui_ImplAllegro5_ProcessEvent(&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			ev.type = FIN_IMGUI;
			return;
		}
	}
	else {
		ImGui_ImplAllegro5_NewFrame();
		ImGui::NewFrame();
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		
		ImGui::Begin("Blockchain Viewer");
		
		ImGui::InputText("Ingresar Directorio", newpath, sizeof(newpath));
		
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Columns(2);
		
		ImGui::BeginChild("Archivos", ImVec2(ImGui::GetColumnWidth()-10, 300));
		ImGui::Text("Archivos");
		ImGui::Separator();

		if (strcmp(newpath, oldpath) != 0) {
			strcpy_s(oldpath, newpath);
			m.restart();
			vector<path> blockchainfiles;
			path p(newpath);

			if (exists(p) && is_directory(p))
				for (directory_iterator it{ p }; it != directory_iterator{}; it++)
					if ((*it).path().filename().extension() == ".json")
						blockchainfiles.push_back((*it).path());

			m.setFiles(blockchainfiles);
			m.setFileIndex(-1);
		}

		bool fileSelected = false;
		int fileIndex = m.getFileIndex();
		for (int i = 0; i < m.getFilesSize(); i++) {
			fileSelected = ImGui::MenuItem((" " + m.getFileName(i)).c_str(), nullptr, (fileIndex == i)?true:false);
			if (fileSelected) {
				m.setFileIndex(i);				
				json j;
				std::ifstream i(m.getFilePath(i).string());
				i >> j;
				m.clearBlockChain();
				saveBlocksFromJson(j);
			}
		}

		ImGui::EndChild();	//Archivos
		ImGui::NextColumn();
		ImGui::BeginChild("Bloques", ImVec2(ImGui::GetColumnWidth()-10, 300));
		ImGui::Text("Bloques");
		ImGui::Separator();

		if (fileIndex != -1) {
			for (int i = 0; i < m.getBlockchainSize(); i++) {
				bool selectedBlock = m.getBlockSelected(i);
				ImGui::Checkbox(("##Bloque " + to_string(i)).c_str(), &selectedBlock);
				m.setBlockSelected(i, selectedBlock);
				m.setBlockShowingInfo(i, true);
				m.setBlockShowingInfo(i, false);
			}
		}

		ImGui::EndChild();	//Bloques

		if (ImGui::Button("Seleccionar todos")) {
			for (int i = 0; i < m.getBlockchainSize(); i++) {
				m.setBlockSelected(i, true);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Quitar seleccion")) {
			for (int i = 0; i < m.getBlockchainSize(); i++) {
				m.setBlockSelected(i, false);
			}
		}
		if (ImGui::Button("Abrir Bloques")) {
			for (int i = 0; i < m.getBlockchainSize(); i++) {
				if (m.getBlockSelected(i)) {
					m.setBlockOpened(i, true);
				}
				m.setBlockSelected(i, false);
			}
		}

		ImGui::Columns();
		ImGui::Separator();
		
		ImGui::End();

		for (int i = 0; i < m.getBlockchainSize(); i++) {
			if (m.getBlockOpened(i)) {
				bool openedBlock = true;
				ImGui::Begin(("Block " + to_string(i)).c_str(), &openedBlock, ImGuiWindowFlags_HorizontalScrollbar);
				m.setBlockOpened(i, openedBlock);

				m.setBlockShowingTree(i, true);
				m.setBlockShowingTree(i, false);

				ImGui::End();
			}
		}

		// Rendering
		ImGui::Render();
		al_clear_to_color(al_map_rgba_f(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
		ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

		al_flip_display();
	}
}