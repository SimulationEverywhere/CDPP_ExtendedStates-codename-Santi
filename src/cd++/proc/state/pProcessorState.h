/*******************************************************************
*
*  DESCRIPTION: class ParProcessorState 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 06/11/2000
*
*******************************************************************/

#ifndef _PPROCESSTATE_H
#define _PPROCESSTATE_H

#include "BasicState.h"
#include "VTime.h"


class ParallelProcessorState : public BasicState {
public:


	VTime next;     
	VTime last;

	ParallelProcessorState();
	virtual ~ParallelProcessorState();

	ParallelProcessorState& operator=(ParallelProcessorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif // _PPROCESSORSTATE_H
