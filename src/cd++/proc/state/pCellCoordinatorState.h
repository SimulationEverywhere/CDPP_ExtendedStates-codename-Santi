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

#ifndef _PCELLCOORDINATORSTATE_H
#define _PCELLCOORDINATORSTATE_H

#include "pCoordinatorState.h"
#include "pprocadm.h"		//Definition of ProcId


/** forward declarations */
class CellState;

class ParallelCellCoordinatorState : public ParallelCoordinatorState {
public:

#ifdef TIME_WARP
	CellState *cellstate;
#endif

	ParallelCellCoordinatorState();
	virtual ~ParallelCellCoordinatorState();

	ParallelCellCoordinatorState& operator=(ParallelCellCoordinatorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif //_PCELLCOORDINATORSTATE_H
