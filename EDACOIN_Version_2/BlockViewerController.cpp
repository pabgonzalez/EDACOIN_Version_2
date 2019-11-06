#include <iostream>
#include <string>
#include <fstream>
#include "BlockViewerController.h"

using namespace std;

void BlockViewerController::update(void* objeto) {
	//printf("Pantalla actualizada\n");
}

void BlockViewerController::cycle() {
	dispatch();
}

void BlockViewerController::getEvent() {
	
}

void BlockViewerController::dispatch() {
	static char oldpath[256] = {};
	static char newpath[256] = {};
	static vector<bool> selectedBlocks;
	static vector<bool> openBlocks;

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
			m.setFileSelected(true);
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
}