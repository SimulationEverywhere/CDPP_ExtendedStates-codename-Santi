/*******************************************************************
*
*  DESCRIPTION: class ParallelSimulator
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 07/11/2000
*
*******************************************************************/

/** include files **/
#include "psimulat.h"          // header
#include "atomic.h"           // class Atomic
#include "pmessage.h"          // class InitMessage
#include "parsimu.h"

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: Simulator
* Description: constructor
********************************************************************/
ParallelSimulator::ParallelSimulator( Atomic *atomic ) 
	: ParallelProcessor( atomic )
{}

void ParallelSimulator::initialize(){
	ParallelProcessor::initialize();
	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;
}

/*******************************************************************
* Function Name: receive
* Description: set the time for the first internal event
********************************************************************/
ParallelProcessor &
	ParallelSimulator::receive( const InitMessage &msg ) 
{
	Atomic &at( static_cast<Atomic &>( model() ) ) ;

	nextChange( VTime::Inf ) ;

	at.initFunction() ;

	DoneMessage done( msg.time(), ParallelProcessor::id(), nextChange(), false ) ;

	send( done, at.parentId() ) ;

	lastChange( msg.time() ) ;

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: call the externalFunction and send done message
********************************************************************/
ParallelProcessor &
	ParallelSimulator::receive( const CollectMessage &msg ) 
{
	//CollectMessage does not have to change 
	//next change or last change!!!

	MASSERT( absoluteNext() == msg.time() );

	Atomic &at( static_cast<Atomic &>( model() ) );

	at.outputFunction( msg ) ;

	DoneMessage done( msg.time(), id(), VTime::Zero, false ) ;

	send( done, at.parentId() ) ;

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: call the Internal or Confluent function
********************************************************************/
ParallelProcessor &
	ParallelSimulator::receive( const InternalMessage &msg ) 
{

	MASSERT( msg.time() >= lastChange() && msg.time() <= absoluteNext() );

	Atomic &at( static_cast<Atomic&>( model() ) ) ;

	if ( msg.time() < absoluteNext() )
	{
		nextChange(absoluteNext() - msg.time());
		at.externalFunction( externalMsgs );
		externalMsgs.eraseAll();

	}
	else if ( msg.time() == absoluteNext() && externalMsgs.size() == 0 )
	{
		nextChange( VTime::Zero );
		at.internalFunction( msg );

	}
	else if ( msg.time() == absoluteNext() && externalMsgs.size() != 0 )
	{
		nextChange( VTime::Zero );
		at.confluentFunction( msg, externalMsgs );
		externalMsgs.eraseAll();
	}

	lastChange( msg.time() );

	DoneMessage done(msg.time(), id(), nextChange(), false);

	send (done, at.parentId() );

	return *this ;
}


/*******************************************************************
* Function Name: allocateState
********************************************************************/
BasicState* 
ParallelSimulator::allocateState() {
	ParallelSimulatorState *state = new ParallelSimulatorState();
	state->modelState =  (AtomicState*) model().allocateState();
	return state;
}
