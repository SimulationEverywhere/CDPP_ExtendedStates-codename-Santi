/*******************************************************************
*
*  DESCRIPTION: class Root
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 02/03/1999 (v2)
*
*******************************************************************/

#ifndef __ROOT_H
#define __ROOT_H

/** include files **/
#include <list>      //Template std::list
#include "event.h"         //class event
#include "process.h"       // class Processor
#include "sprocadm.h"      // StandAloneProcessorAdmin::RootId 

/** forward declarations **/
class Coupled ;
class ParallelMainSimulator ;
class Port ;
class VTime ;

/** declarations **/
typedef std::list<Event> EventList;

class Root : public Processor
{
public:   
	static Root &Instance();
	Root &initialize();
	Root &simulate();
	Root &stop();
	Root &addExternalEvent( const VTime &, const Port &, const Real & ) ;
	Processor &receive( const OutputMessage & );
	Processor &receive( const DoneMessage &);
	Root &stopTime( const VTime & ) ;
	const VTime &stopTime() const ;
	const EventList &events() const ;
	
	const std::string description() const ;
	
private:
	friend class ParallelMainSimulator;
	friend class StandAloneMainSimulator;
	friend class ParallelProcessorAdmin ;
	friend class StandAloneProcessorAdmin;
	Root( const ProcId & = StandAloneProcessorAdmin::rootId ) ;	  // Default constructor
	Processor *child ;
	VTime timeStop ;
	VTime lastChg ;
	EventList externalEvents;
	
	Coupled &top() ;
	const Coupled &top() const ;
	
};	// class Root


/** inline **/
inline
	const EventList &Root::events() const
{
	return externalEvents ;
}

inline
	Root &Root::stopTime( const VTime &t )
{
	timeStop = t ;
	return *this ;
}

inline
	const VTime &Root::stopTime() const
{
	return timeStop ;
}

inline
	const std::string Root::description() const
{
	return "Root" ;
}

#endif   //__ROOT_H 
