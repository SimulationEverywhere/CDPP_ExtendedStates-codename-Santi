/*******************************************************************
*
*  DESCRIPTION: class ParallelSimulatorState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 09/11/2000
*
*******************************************************************/

#ifndef _PSIMULATORSTATE_H
#define _PSIMULATORSTATE_H

#include "pProcessorState.h"
#include "atomicstate.h"


class ParallelSimulatorState : public ParallelProcessorState {
public:


	AtomicState* modelState;

	ParallelSimulatorState(): modelState(NULL){};
	virtual ~ParallelSimulatorState();

	ParallelSimulatorState& linkModelState(AtomicState* mdlSt)
	{modelState = mdlSt; return *this;}

	ParallelSimulatorState& operator=(ParallelSimulatorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif //_PSIMULATORSTATE_H
