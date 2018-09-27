/*******************************************************************
*
*  DESCRIPTION: class ParallelRootState
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 29/01/2001
*
*******************************************************************/

#include "pRootState.h"

using namespace std;


/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelRootState& 
ParallelRootState::operator=(ParallelRootState& thisState) {
	
	(ParallelProcessorState &)*this  = (ParallelProcessorState &) thisState;
	(this->next) = (thisState.next);
	(this->eventsCursor) = (thisState.eventsCursor);
	
	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelRootState::copyState(BasicState *rhs) {
	*this = *((ParallelRootState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelRootState::getSize() const {
	return sizeof(ParallelRootState);
}
