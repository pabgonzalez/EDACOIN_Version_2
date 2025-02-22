#pragma once
#include "Observer.h"
#include <vector>
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_font.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"

#define FIN_IMGUI ALLEGRO_EVENT_DISPLAY_CLOSE

class ImGuiObserverManager {
public:
	ImGuiObserverManager();
	~ImGuiObserverManager();

	void addObserver(Observer* o);
	void removeObserver(int i);
	bool getExit() { return (ev.type == FIN_IMGUI); }
	bool getError() { return error; }

	void cycle();
private:
	bool allegroInit();
	void allegroDestroy();

	ALLEGRO_DISPLAY* display;
	vector<Observer*> observers;
	bool error;
	ALLEGRO_EVENT_QUEUE* eventQueue;
	ALLEGRO_EVENT ev;
	bool queueEmpty;
};