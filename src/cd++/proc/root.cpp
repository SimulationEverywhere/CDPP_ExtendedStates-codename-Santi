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

/** include files **/
#include "evaldeb.h"	// ShowVirtualTimeWhenFinish
#include "root.h"       // base
#include "msgadm.h"     // class MessageAdm
#include "message.h"    // class Message
#include "pmodeladm.h"   // class SingleParallelModelAdm
#include "coupled.h"    // class Coupled
#include "parsimu.h"   // class Simulator

using namespace std;

/** public **/
/*******************************************************************
* Function Name: Instance
* Description: 
********************************************************************/
Root &Root::Instance()
{
	static bool instance( false );
	
	if( !instance )
	{
		SingleProcessorAdmin::Instance().generateRoot()  ;
		instance = true ;
	}
	
	return static_cast<Root &>( SingleProcessorAdmin::Instance().processor(StandAloneProcessorAdmin::rootId ) ) ; 
}

/*******************************************************************
* Function Name: Root
* Description: constructor Default
********************************************************************/
Root::Root( const ProcId &id )
	: child( NULL )
{
	this->id( id ) ;
	
	Coupled &coupled( SingleParallelModelAdm::Instance().newCoupled( "top" ) ) ;
	coupled.parentId( this->id() );
	
	Processor &proc( SingleProcessorAdmin::Instance().processor( coupled.id() ) ) ;
	child = &proc ;
}

/*******************************************************************
* Function Name: initialize
* Description: clean event List and output device
********************************************************************/
Root &Root::initialize()
{
	lastChange( VTime::Zero );
	nextChange( VTime::Zero );
	externalEvents.erase( externalEvents.begin(), externalEvents.end() ) ;
	stopTime( VTime::Inf ) ;
	
	return *this;
}

/*******************************************************************
* Function Name: start
* Description: begin the simulation
********************************************************************/
Root &Root::simulate()
{
	externalEvents.sort();
	InitMessage initMsg( VTime::Zero, id() ) ;
	SingleMsgAdm::Instance().send( initMsg, top().id() ) ;
	SingleMsgAdm::Instance().run() ;
	return *this;
}

/*******************************************************************
* Function Name: stop
* Description: stop the simulation
********************************************************************/
Root &Root::stop()
{
	if (ShowVirtualTimeWhenFinish().Active())
		cerr << "\n" << lastChg << "\n";
	
	SingleMsgAdm::Instance().stop() ;
	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: send the time and value of the message to the output device 
********************************************************************/
Processor &Root::receive( const OutputMessage &msg )
{
	ParallelMainSimulator::Instance().outputStream() 
		<< msg.time().asString() << " " << msg.port().name() << " " << Real(msg.value()) << endl ;
	
	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: advance the time and go on with the simulation
********************************************************************/
Processor  &Root::receive( const DoneMessage &msg )
{
	lastChg = msg.time();
	nextChange( msg.nextChange() );
	
	if( externalEvents.empty() && nextChange()==VTime::Inf )
		stop() ;
	else
	{
		// there is any event?
		Event ev( VTime::Inf ) ;
		VTime nt( msg.time() + nextChange() ) ;
		Message *pMsg;
		
		if( !externalEvents.empty() )
			ev = externalEvents.front() ;
		
		if( ev.time <= nt )
		{
			pMsg = new ExternalMessage( ev.time, id(), *ev.port, ev.value.value() );
			externalEvents.pop_front() ;
		} else
			pMsg = new InternalMessage( nt, id() ) ;
		
		if( stopTime() < pMsg->time() )
			stop() ;
		else
			SingleMsgAdm::Instance().send( *pMsg, top().id() );
		
		delete pMsg;
	}
	return *this;
}

/*******************************************************************
* Function Name: addExternalEvent
* Description: add external event
********************************************************************/
Root &Root::addExternalEvent( const VTime &t, const Port &p, const Real &r )
{
	externalEvents.push_back( Event(t, &p, r) );
	return *this;
}

/*******************************************************************
* Function Name: top
********************************************************************/
Coupled &Root::top() 
{
	return static_cast<Coupled &>( child->model() );
}

/*******************************************************************
* Function Name: top
********************************************************************/
const Coupled &Root::top() const 
{
	return static_cast<const Coupled &>( child->model() ) ;
}
