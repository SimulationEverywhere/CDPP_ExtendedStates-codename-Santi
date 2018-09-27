/*******************************************************************
*
*  DESCRIPTION: class ParallelMasterCoordinatorState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 09/11/2000
*
*******************************************************************/

#ifndef _PMCOORDINATORSTATE_H
#define _PMCOORDINATORSTATE_H

#include "pCoordinatorState.h"
#include "pprocadm.h"		//Definition of ProcId

class ParallelMCoordinatorState : public ParallelCoordinatorState {
public:
	
	DependantList* slaves;
	
	ParallelMCoordinatorState();
	virtual ~ParallelMCoordinatorState();
	
	ParallelMCoordinatorState& operator=(ParallelMCoordinatorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif //_PMCOORDINATORSTATE_H
