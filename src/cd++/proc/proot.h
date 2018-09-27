/*******************************************************************
*
*  DESCRIPTION: class ParallelRoot
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 11/01/2001
*
*******************************************************************/

#ifndef __ROOT_H
#define __ROOT_H

/** include files **/
#include <list>      //Template std::list
#include "event.h"         //class event
#include "eventlist.h"
#include "prootmodel.h"
#include "pprocess.h"       // class Processor
#include "pRootState.h"	// class ParallelRootState
#include "pprocadm.h"       // ParallelProcessorAdmin::rootId 

/** forward declarations **/
class Coupled ;
class ParallelMainSimulator ;
class Port ;
class VTime ;

class ParallelRoot : public ParallelProcessor
{
public:   
	static ParallelRoot &Instance();
	
	//Functions that are required by Warped
	void initialize();
	
	ParallelRoot &rootInitialize();
	ParallelRoot &rootSimulate();
	ParallelRoot &rootStop();
	ParallelRoot &addExternalEvent( const VTime &, const Port &, const AbstractValue & ) ;
	
	ParallelProcessor &receive( const BasicOutputMessage * );
	ParallelProcessor &receive( const DoneMessage &);
	
	ParallelRoot &stopTime( const VTime & ) ;
	
	const VTime &stopTime() const ;
	const EventList &events() const ;
	
	const std::string description() const ;
	BasicState* allocateState();
	
	bool createOutput;
	int outputIndex;
	
private:
	friend class ParallelProcessorAdmin ;
	
	ParallelRoot( RootModel*) ;	  // Default constructor
	
	//State shortcuts
	void sendMsgType ( ParallelRootState::nextMsgType msgType);
	ParallelRootState::nextMsgType sendMsgType ();
	
	//Event List Management shortcuts
	EventList::iterator currentEvent();
	void eventsBegin();
	void eventsMoveNext();
	bool endOfEvents();
	
	VTime timeStop ;
	EventList externalEvents;
	
	ProcId topMasterId;
	
	//lastChg is defined in the Processors State
	//VTime lastChg ;
	
};	// class Root


/** inline **/
inline
	const EventList &ParallelRoot::events() const
{
	return externalEvents ;
}

inline
	ParallelRoot &ParallelRoot::stopTime( const VTime &t )
{
	timeStop = t ;
	return *this ;
}

inline
	const VTime &ParallelRoot::stopTime() const
{
	return timeStop ;
}

inline
	const std::string ParallelRoot::description() const
{
	return "ParallelRoot" ;
}

/*******************************************************************
* Function Name: currentEvent
* Description: retrieve the current event iterator from the Root State
********************************************************************/
inline
EventList::iterator ParallelRoot::currentEvent(){
	return ((ParallelRootState *)state->current)->eventsCursor;
}

/*******************************************************************
* Function Name: eventBegin
* Description: move the current event iterator to the first event
********************************************************************/
inline
void ParallelRoot::eventsBegin() {
	(((ParallelRootState *)state->current)->eventsCursor) = externalEvents.begin();
}

/*******************************************************************
* Function Name: nextEvent
* Description: move the current event iterator to the next event
********************************************************************/
inline
void ParallelRoot::eventsMoveNext() {
	((ParallelRootState *)state->current)->eventsCursor++;
}


/*******************************************************************
* Function Name: endOfEvents
* Description: returns true if the last external events has been processed
********************************************************************/
inline
bool ParallelRoot::endOfEvents() {
	return ((((ParallelRootState *)state->current)->eventsCursor) == externalEvents.end());
}

/*******************************************************************
* Function Name: allocateState
* Description: 
********************************************************************/
inline
BasicState*	ParallelRoot::allocateState() {
	return new ParallelRootState;	
	
}

/*******************************************************************
* Function Name: State Shortcuts
* Description: 
********************************************************************/

inline
void ParallelRoot::sendMsgType ( ParallelRootState::nextMsgType msgType) {
	((ParallelRootState *)state->current)->next = msgType;
}

inline
ParallelRootState::nextMsgType ParallelRoot::sendMsgType () {
	return ((ParallelRootState *)state->current)->next;
}

#endif   //__ROOT_H 
