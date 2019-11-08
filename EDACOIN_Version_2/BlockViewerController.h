#pragma once
#include "Controller.h"
#include "BlockchainModel.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

using namespace std;

class BlockViewerController :public Controller {
public:
	BlockViewerController(BlockchainModel& modelo) :Controller(modelo) {}
	//
	BlockViewerController(ALLEGRO_DISPLAY** disp, BlockchainModel& modelo);
	//
	void update(void*);
	void cycle(void);

private:
	void getEvent(void);
	void dispatch(void);
};