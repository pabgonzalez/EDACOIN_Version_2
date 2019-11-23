#pragma once
#include "Controller.h"
#include "json.hpp"
#include <allegro5/allegro.h>
#include <boost/filesystem.hpp>

using json = nlohmann::json;

#define FIN_IMGUI ALLEGRO_EVENT_DISPLAY_CLOSE
#define NO_EVENT ALLEGRO_EVENT_MOUSE_BUTTON_UP

typedef ALLEGRO_EVENT Evento;
typedef ALLEGRO_EVENT_QUEUE* ColaDeEvento;

Block createBlock(json j);
vector<Transaction> createTxVector(json j);
Transaction createTx(json j);
Filter createFilter(json j);

class JsonController :public Controller {
public:
	JsonController(BlockchainModel& modelo);
	~JsonController();
	void update(void*);
	void cycle(void);

private:
	void getEvent(void);
	void dispatch(void);
	void saveBlocksFromJson(json j);

};