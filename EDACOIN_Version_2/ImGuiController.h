#pragma once
#include "Controller.h"
#include "BlockchainModel.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "json.hpp"

#define FIN_IMGUI ALLEGRO_EVENT_DISPLAY_CLOSE
#define NO_EVENT ALLEGRO_EVENT_MOUSE_BUTTON_UP

using namespace std;

typedef ALLEGRO_EVENT Evento;
typedef ALLEGRO_EVENT_QUEUE * ColaDeEvento;
typedef ALLEGRO_DISPLAY* Display;

class BlockViewerController:public Controller {
public:
	BlockViewerController(ALLEGRO_DISPLAY** disp, BlockchainModel& modelo);
	~BlockViewerController();
	void update(void*);
	void run(void);
	void cycle(void);

private:
	void getEvent(void);
	void dispatch(void);

	bool queueEmpty;
	Evento ev;
	ColaDeEvento eventQueue;
	ALLEGRO_DISPLAY* display;
};