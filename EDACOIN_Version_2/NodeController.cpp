#include "NodeController.h"

void NodeController::cycle(void) {
	for (int i = 0; i < fullNodes.size(); i++) {
		fullNodes[i]->acceptConnection();
		fullNodes[i]->readRequest();
		fullNodes[i]->writeResponse();
	}
}