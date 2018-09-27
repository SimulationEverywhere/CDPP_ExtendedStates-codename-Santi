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

#ifndef _PROOTSTATE_H
#define _PROOTSTATE_H

#include "pProcessorState.h"
#include "eventlist.h"

class ParallelRootState :public ParallelProcessorState {
public:
	
	
	//The root processor has to hold the External events std::list
	//and has to "remember" which is the next type of message to send:
	//a CollectMessage or an InternalMessage
	
	enum nextMsgType 
	{
		CollectMsg,
		InternalMsg
	};
	
	nextMsgType next;
	
	//To manage the external events, we only need a pointer to the 
	//current element, we do not need the whole std::list in the state
	EventList::iterator eventsCursor;
	
	/*******************************************************************
	* Function Name: constructor
	********************************************************************/
	inline
	ParallelRootState() {
	}

	/*******************************************************************
	* Function Name: destructor
	********************************************************************/
	inline virtual
	~ParallelRootState() {
	}

	
	ParallelRootState& operator=(ParallelRootState& thisState); //Assignment
	void copyState(BasicState *);
	int  getSize() const;
};


#endif //_PROOTSTATE_H
