#pragma once
#include <allegro5/allegro.h>
#include "Observer.h"
#include "BlockchainModel.h"

class ImGuiViewer : public Observer {
public:
	void update(void* model);
	void cycle(void) {};
	ALLEGRO_BITMAP* drawTree(vector<MerkleNode>);
private:
	vector<ALLEGRO_BITMAP*> treeBitmaps;
};
