/*******************************************************************
*
*  DESCRIPTION: class ParallelCoordinatorState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 09/11/2000
*
*******************************************************************/

#ifndef _PCOORDINATORSTATE_H
#define _PCOORDINATORSTATE_H

#include "pProcessorState.h"
#include "pprocadm.h"		//Definition of ProcId


class ParallelCoordinatorState : public ParallelProcessorState {
public:
	
	
	typedef std::map < ProcId, VTime, std::less <ProcId> > DependantList;
	
	DependantList* dependants;
	
	ParallelCoordinatorState();
	virtual ~ParallelCoordinatorState();
	
	ParallelCoordinatorState& operator=(ParallelCoordinatorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif //_PCOORDINATORSTATE_H
