#include "LocalNodes.h"
#include <math.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define RAD 120
#define THICKNESS 10
#define FONT_SIZE 20
#define SIZEOFGRAPH 1000
#define PX_PER_UNIT (SIZEOFGRAPH/5)


LocalNodes::LocalNodes() {
	selectedNodeIndex = -1;
	selectedIsFull = false;
}

LocalNodes::~LocalNodes() {
	for (unsigned int i = 0; i < fullNodes.size(); i++) {
		delete fullNodes[i].fullNode;
	}
	for (unsigned int i = 0; i < spvNodes.size(); i++) {
		delete spvNodes[i].spvNode;
	}
}

bool LocalNodes::isAnyNodeSelected() {
	if (selectedNodeIndex >= 0) {
		if (selectedIsFull && fullNodes.size() > selectedNodeIndex){
			return true;
		}
		if (!selectedIsFull && spvNodes.size() > selectedNodeIndex) {
			return true;
		}
	}
	return false;
}

void LocalNodes::setSelectedNode(int index, bool isFullNode) {
	selectedNodeIndex = index;
	selectedIsFull = isFullNode;
	notifyAllObservers();
}

void LocalNodes::addFullNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes) {
	FullNode* full = new FullNode(socket, ID, neighbourNodes);
	appendNode(full);
	notifyAllObservers();
}
void LocalNodes::addSPVNode(SocketType socket, string ID, map<string, SocketType> neighbourNodes) {
	SPVNode* spv = new SPVNode(socket, ID, neighbourNodes);
	appendNode(spv);
	notifyAllObservers();
}

void LocalNodes::appendNode(FullNode* node) {
	fullNodes.push_back(FullNodeInfo(node));
	notifyAllObservers();
}

void LocalNodes::appendNode(SPVNode* node) {
	spvNodes.push_back(SPVNodeInfo(node));
	notifyAllObservers();
}

ALLEGRO_BITMAP* FullNodeInfo::graphNeighbours(void) {
	ALLEGRO_BITMAP* backup = al_get_target_bitmap();
	ALLEGRO_FONT* font = NULL;
	ALLEGRO_BITMAP* graph = NULL;
	char fontLoc[] = "arial.ttf";
	graph = al_create_bitmap(PX_PER_UNIT * 5, PX_PER_UNIT * 5);
	al_set_target_bitmap(graph);
	al_clear_to_color(al_map_rgb(255, 255, 255));
	font = al_load_ttf_font(fontLoc, FONT_SIZE, 0);
	map<string, SocketType>::iterator it = fullNode->getNeighbours().begin();
	al_draw_filled_circle(PX_PER_UNIT * 5 / 2, PX_PER_UNIT * 5 / 2, 60, al_map_rgb(100, 255, 100));
	al_draw_text(font, al_map_rgb(0, 0, 0), (PX_PER_UNIT * 5 / 2) , (PX_PER_UNIT * 5 / 2),NULL,fullNode->getNodeID().c_str());
	for (int i = 0; i <= (fullNode->getNeighbours().size()); i++, it++) {
		double angle = i * 2 * 3.14 / fullNode->getNeighbours().size();
		al_draw_filled_circle((PX_PER_UNIT * 5 / 2) + RAD * cos(angle), (PX_PER_UNIT * 5 / 2) - RAD * sin(angle), 60, al_map_rgb(100, 255, 100));
		al_draw_line((PX_PER_UNIT * 5 / 2), (PX_PER_UNIT * 5 / 2),(PX_PER_UNIT * 5 / 2) + RAD * cos(angle), (PX_PER_UNIT * 5 / 2) - RAD * sin(angle), al_map_rgb(255, 0, 0), THICKNESS);
		al_draw_text(font,al_map_rgb(0,0,0),(PX_PER_UNIT * 5 / 2) + RAD * cos(angle), (PX_PER_UNIT * 5 / 2) - RAD * sin(angle),NULL, it->first.c_str());
	}
	al_set_target_bitmap(backup);
	return graph;
}