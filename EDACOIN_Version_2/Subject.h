#pragma once

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Observer.h"
#include<iostream>
#include <vector>

/*******************************************************************************
 * CLASS PROTOTYPE
 ******************************************************************************/
class Subject
{
public:
	void attach(Observer& obs);
	void dettach(vector<Observer*>::iterator pos);
protected:
	vector<Observer*> observers;
	void notifyAllObservers(void);
};