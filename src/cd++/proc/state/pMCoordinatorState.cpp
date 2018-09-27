/*******************************************************************
*
*  DESCRIPTION: class ParallelMCoordinatorState
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 24/01/2001
*
*******************************************************************/

#include "pMCoordinatorState.h"

using namespace std;

/*******************************************************************
* Function Name: constructor
********************************************************************/
ParallelMCoordinatorState::ParallelMCoordinatorState(): slaves(NULL) {
}

/*******************************************************************
* Function Name: destructor
********************************************************************/
ParallelMCoordinatorState::~ParallelMCoordinatorState() {
	if ( slaves )
		delete slaves;
}

/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelMCoordinatorState& 
ParallelMCoordinatorState::operator=(ParallelMCoordinatorState& thisState) {
	
	(ParallelCoordinatorState &)*this  = (ParallelCoordinatorState &) thisState;
	
	//Precondition (this will always be done by allocateState)
	//slaves is not NULL
	*(this->slaves) = *thisState.slaves;
	
	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelMCoordinatorState::copyState(BasicState *rhs) {
	*this = *((ParallelMCoordinatorState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelMCoordinatorState::getSize() const {
	return sizeof(ParallelMCoordinatorState);
}
