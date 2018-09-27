/*******************************************************************
*
*  DESCRIPTION: class AtomicState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 09/11/2000
*
*******************************************************************/

#include "atomicstate.h"

using namespace std;

/*******************************************************************
* Function Name: operator =
********************************************************************/
AtomicState& 
AtomicState::operator=(AtomicState& thisState){

	(ModelState &)*this  = (ModelState &) thisState;
	this->st = thisState.st;

	return *this;	
}


/******************************************************************
* Function Name: copyState
********************************************************************/
void 
AtomicState::copyState(BasicState * rhs){
	*this = *((AtomicState *) rhs);
};

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
AtomicState::getSize() const {
	return sizeof(AtomicState);	
};




