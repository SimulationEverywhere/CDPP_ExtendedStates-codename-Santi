/*******************************************************************
*
*  DESCRIPTION: class ParallelCoordinatorState
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 18/01/2001
*
*******************************************************************/

#include "pCoordinatorState.h"

using namespace std;

/*******************************************************************
* Function Name: constructor
********************************************************************/
ParallelCoordinatorState::ParallelCoordinatorState():dependants(NULL) {
}

/*******************************************************************
* Function Name: destructor
********************************************************************/
ParallelCoordinatorState::~ParallelCoordinatorState() {
	
	if( dependants )
		delete dependants;
}


/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelCoordinatorState& 
ParallelCoordinatorState::operator=(ParallelCoordinatorState& thisState) {
	
	(ParallelProcessorState &)*this  = (ParallelProcessorState &) thisState;
	
	//Precondition (this will always be done by allocateState)
	//slaveList is not NULL
	*(this->dependants) = *thisState.dependants;
	
	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelCoordinatorState::copyState(BasicState *rhs) {
	*this = *((ParallelCoordinatorState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelCoordinatorState::getSize() const {
	return sizeof(ParallelCoordinatorState);
}
