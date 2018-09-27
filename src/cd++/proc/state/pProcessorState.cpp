/*******************************************************************
*
*  DESCRIPTION: class ParProcessorState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 07/11/2000
*
*******************************************************************/

#include "pProcessorState.h"

using namespace std;

/*******************************************************************
* Function Name: constructor
********************************************************************/
ParallelProcessorState::ParallelProcessorState() {
}

/*******************************************************************
* Function Name: destructor
********************************************************************/
ParallelProcessorState::~ParallelProcessorState() {
}

/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelProcessorState& 
ParallelProcessorState::operator=(ParallelProcessorState& thisState) {

	(BasicState &)*this  = (BasicState &) thisState;
	this->next = thisState.next;
	this->last = thisState.last;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelProcessorState::copyState(BasicState *rhs) {
	*this = *((ParallelProcessorState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelProcessorState::getSize() const {
	return sizeof(ParallelProcessorState);
}

