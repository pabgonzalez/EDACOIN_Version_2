/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Subject.h"

/*******************************************************************************
 * CASS METHODS DEFINITIONS
 ******************************************************************************/
void Subject::attach(Observer& obs) { observers.push_back(&obs); }
void Subject::dettach(vector<Observer*>::iterator pos) { observers.erase(pos); }

void Subject::notifyAllObservers(void)
{
	for (Observer* o : observers)
	{
		o->update(this);
	}
}