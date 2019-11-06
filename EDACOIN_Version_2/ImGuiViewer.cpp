#include "ImGuiViewer.h"
#include "BlockchainModel.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define THICKNESS 10
#define FONT_SIZE 20
#define SIDE 800
#define PX_PER_UNIT	75
#define PAR 0
#define IMPAR 1
#define TEXT_OFFSETX 45
#define TEXT_OFFSETY 12

void ImGuiViewer::update(void* model) {
	BlockchainModel* m = (BlockchainModel*)model;

	if (m->getBlockchainSize() == 0 && treeBitmaps.size() > 0)
		treeBitmaps.clear();

	if (m->getBlockchainSize() > treeBitmaps.size()){
		treeBitmaps.clear();	//Es solo por precaucion
		for (int i = 0; i < m->getBlockchainSize(); i++) {
			treeBitmaps.push_back(NULL);
		}
	}

	for (int i = 0; i < m->getBlockchainSize(); i++) {
		if (m->getBlockShowingInfo(i)) {
			ImGui::SameLine();
			if (ImGui::CollapsingHeader(("Bloque " + to_string(i)).c_str())) {
				ImGui::Separator();
				ImGui::Columns(2);
				ImGui::Text("ID del bloque");
				ImGui::Text("ID del bloque anterior");
				ImGui::Text("Cantidad de transacciones");
				ImGui::Text("Numero de bloque");
				ImGui::Text("Nonce");
				ImGui::Text("Merkleroot");
				ImGui::Text("Merkle Root Calculado");
				ImGui::Text("Verificacion del Merkle Root");
				ImGui::NextColumn();
				ImGui::Text(m->getBlockId(i).c_str());
				ImGui::Text(m->getPreviousBlockId(i).c_str());
				ImGui::Text(to_string(m->getBlockTransactionNumber(i)).c_str());
				ImGui::Text(to_string(m->getBlockHeight(i)).c_str());
				ImGui::Text(to_string(m->getBlockNonce(i)).c_str());
				ImGui::Text(m->getBlockMerkleRoot(i).c_str());
				ImGui::Text(m->getBlockCalculatedMerkleRoot(i).c_str());
				ImGui::Text((m->validateMerkleRoot(i) == true) ? "Correcto" : "Incorrecto");
				ImGui::Columns();
				ImGui::Separator();
			}
		}
		if (m->getBlockShowingTree(i)) {
			//Mostrar el arbol
			
			if (treeBitmaps[i] == NULL) {
				treeBitmaps[i] = drawTree(m->getMerkleTree(i));
			}
			
			ImGui::Image(treeBitmaps[i], ImVec2(al_get_bitmap_width(treeBitmaps[i]), al_get_bitmap_height(treeBitmaps[i])));
		}
	}
}

ALLEGRO_BITMAP* ImGuiViewer:: drawTree(vector<MerkleNode> merkTree) {
	ALLEGRO_BITMAP* backup = al_get_target_bitmap();
	ALLEGRO_BITMAP* tree = NULL;
	ALLEGRO_FONT* font = NULL;
	char fontLoc[] = "arial.ttf";
	int a = merkTree.front().level;
	tree = al_create_bitmap(PX_PER_UNIT*pow(2.0, a + 1), PX_PER_UNIT* (2 * a + 2));
	al_set_target_bitmap(tree);

	al_clear_to_color(al_map_rgb(255, 255, 255));
	
	font = al_load_ttf_font(fontLoc, FONT_SIZE, 0);
	int ux = PX_PER_UNIT;//SIDE / (pow(2.0, a+1));	//Unidad en x
	int uy = PX_PER_UNIT;//SIDE / (2 * a + 2);		//Unidad en y
	int leafIndex = 0;
	int currentLeafs = 0;
	for (int j = a; j >= 0; j--) {
		while (merkTree[leafIndex].level == j) {
			currentLeafs++;
			if (leafIndex != merkTree.size() - 1)
				leafIndex++;
			else
			{
				leafIndex++;
				break;
			}
		}
		for (int i = 0; i < currentLeafs; i++) {

			ALLEGRO_COLOR fillcolor = al_map_rgb(100, 255, 100);
			if (merkTree[leafIndex - currentLeafs + i].isReal == false) {
				fillcolor = al_map_rgb(100, 100, 255);
			}
			if (j != 0) {
				al_draw_line((1 + 2 * i) * pow(2, a - j) * ux, (1 + 2 * j) * uy, (1 + 2 * (i / 2)) * pow(2, (a - j + 1)) * ux, (2 * j - 1) * uy, al_map_rgb(255, 0, 0), THICKNESS);
			}
			al_draw_filled_circle((1 + 2 * i) * pow(2, a - j) * ux, (1 + 2 * j) * uy, 60, fillcolor);

			al_draw_text(font, al_map_rgb(0, 0, 0), (1 + 2 * i) * pow(2, a - j) * ux - TEXT_OFFSETX, (1 + 2 * j) * uy - TEXT_OFFSETY, NULL, merkTree[leafIndex - currentLeafs + i].newIDstr.c_str());
		}
		currentLeafs = 0;
	}
	
	al_set_target_bitmap(backup);
	return tree;
}











