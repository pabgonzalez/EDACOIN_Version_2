#pragma once
#define ALLEGRO_STATICLINK
#include "Observer.h"
#include "BlockchainModel.h"
#include <allegro5\allegro.h>

class Controller : public Observer
{
public:
	//Controller() {}
	Controller(BlockchainModel& mod) :m(mod) {}
	virtual void update(void*) = 0;
	virtual void cycle(void) = 0;
	BlockchainModel* getModel() { return &m; }
protected:
	virtual void getEvent(void) = 0;
	virtual void dispatch(void) = 0;
	BlockchainModel& m;
};
