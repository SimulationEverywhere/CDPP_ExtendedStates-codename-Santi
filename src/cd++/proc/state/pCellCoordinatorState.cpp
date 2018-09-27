/*******************************************************************
*
*  DESCRIPTION: class ParallelCellCoordinatorState
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 15/02/2001
*
*******************************************************************/

#include "pCellCoordinatorState.h"

using namespace std;

/*******************************************************************
* Function Name: constructor
********************************************************************/
ParallelCellCoordinatorState::ParallelCellCoordinatorState()
#ifdef TIMEWARP
:cellstate(NULL)
#endif
{

}

/*******************************************************************
* Function Name: destructor
********************************************************************/
ParallelCellCoordinatorState::~ParallelCellCoordinatorState() {

#ifdef TIME_WARP	
	if( cellstate )
		delete cellstate;
#endif

}

/*******************************************************************
* Function Name: operator =
********************************************************************/
ParallelCellCoordinatorState& 
ParallelCellCoordinatorState::operator=(ParallelCellCoordinatorState& thisState) {

	(ParallelCoordinatorState &)*this  = (ParallelCoordinatorState &) thisState;

	//Precondition: cellstate is not null!
#ifdef TIME_WARP
	*(this->cellstate) = *thisState.cellstate;
#endif

	return *this;	
}

/*******************************************************************
* Function Name: copyState
********************************************************************/
void 
ParallelCellCoordinatorState::copyState(BasicState *rhs) {
	*this = *((ParallelCellCoordinatorState *) rhs);
}

/*******************************************************************
* Function Name: getSize
********************************************************************/
int  
ParallelCellCoordinatorState::getSize() const {
	return sizeof(ParallelCellCoordinatorState);
}
