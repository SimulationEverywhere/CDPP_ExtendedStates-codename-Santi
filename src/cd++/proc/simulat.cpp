/*******************************************************************
*
*  DESCRIPTION: class Simulator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

/** include files **/
#include "simulat.h"          // header
#include "atomic.h"           // class Atomic
#include "message.h"          // class InitMessage
#include "msgadm.h"           // SingleMsgAdmin

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: Simulator
* Description: constructor
********************************************************************/
Simulator::Simulator( Atomic *atomic ) 
: Processor( atomic )
{}

/*******************************************************************
* Function Name: receive
* Description: set the time for the first internal event
********************************************************************/
Processor &Simulator::receive( const InitMessage &msg ) 
{
	Atomic &at( static_cast<Atomic &>( model() ) ) ;

	nextChange( VTime::Inf ) ;

	at.initFunction() ;

	DoneMessage done( msg.time(), Processor::id(), nextChange() ) ;

	SingleMsgAdm::Instance().send( done, at.parentId() ) ;

	lastChange( msg.time() ) ;

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: call the externalFunction and send done message
********************************************************************/
Processor &Simulator::receive( const ExternalMessage &msg ) 
{
	Atomic &at( static_cast<Atomic &>( model() ) );

	MASSERT( at.absoluteNext() >= msg.time() ) ;

	nextChange( at.absoluteNext() - msg.time() ) ;

	at.externalFunction( msg ) ;

	DoneMessage done( msg.time(), this->Processor::id(), nextChange() ) ;

	SingleMsgAdm::Instance().send( done, at.parentId() ) ;

	lastChange( msg.time() ) ;

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: call to the OutputFunction and InternalFunction
********************************************************************/
Processor &Simulator::receive( const InternalMessage &msg ) 
{
	Atomic &at( static_cast<Atomic&>( model() ) ) ;

	MASSERT( at.absoluteNext() == msg.time() ) ;

	nextChange( VTime::Zero ) ;

	at.outputFunction( msg ) ;

	at.internalFunction( msg ) ;

	DoneMessage done( msg.time(), this->Processor::id(), nextChange() ) ;

	SingleMsgAdm::Instance().send( done, at.parentId() ) ;

	lastChange( msg.time() ) ;

	return *this ;
}

/*******************************************************************
* Function Name: id
********************************************************************/
Processor &Simulator::id( const ProcId &pid )
{
	this->Processor::id( pid ) ;

	MASSERT( pmodel() ) ;

	pmodel()->id( pid ) ;

	return *this ;
}
