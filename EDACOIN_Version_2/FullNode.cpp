#include "FullNode.h"
#include <chrono>
#include <thread>

using namespace std;

FullNode::FullNode() {
	pingStatus = false;
	timer = (rand() % 999) * 10 + 10;;
	state = IDLE;
}
void FullNode::p2pNetFSM() {
	switch (state) {
	case IDLE:
		if (pingStatus == true)
			state = WAITING_LAYOUT;
		else {
			this_thread::sleep_for(chrono::milliseconds(1));
			if (!(--timer))
				state = COLLECTING_NETWORK_MEMBERS;
		}
		break;
	case WAITING_LAYOUT:
		//epera mensaje NetwokLayout
		//responde HTTP OK
		//agrega vecinos
	case COLLECTING_NETWORK_MEMBERS:
		//envia PING a cada nodo
		//si NetworkNotReady agrega nodo a lista
		//si NetworkReady toma lista y conecta con la actual
	case NETWORK_CREATED:
		//
	}
}