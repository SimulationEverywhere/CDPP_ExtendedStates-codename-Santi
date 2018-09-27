/*******************************************************************
*
*  DESCRIPTION: class ParllelSimulatorState
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 09/11/2000
*
*******************************************************************/

#include "pSimulatorState.h"

using namespace std;

/*******************************************************************
* Function Name: destructor
********************************************************************/

ParallelSimulatorState::~ParallelSimulatorState() {
	if ( modelState ) 
		delete modelState;
}

/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelSimulatorState& 
ParallelSimulatorState::operator=(ParallelSimulatorState& thisState) {

	(ParallelProcessorState &)*this  = (ParallelProcessorState &) thisState;

	//If a modelState has been initialized, copy it.
	if ( modelState )
		this->modelState->copyState(thisState.modelState); 	

	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelSimulatorState::copyState(BasicState *rhs) {
	*this = *((ParallelSimulatorState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelSimulatorState::getSize() const {
	return sizeof(ParallelSimulatorState);
}
