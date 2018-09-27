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

#ifndef _PPROCESSSTATE_H
#define _PPROCESSTATE_H

#include "BasicState.h"
#include "VTime.h"


class ParProcessorState : public BasicState {
public:
	
	ParProcessorState();
	virtual ~ParProcessorState();
	
	VTime next;     
	VTime last;
	
	void serialize(ofstream* ckFile, int mysize);
	void deserialize(ifstream* inFile);
	
	ParProcessorState& operator=(ParProcessorState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif _PPROCESSORSTATE_H
