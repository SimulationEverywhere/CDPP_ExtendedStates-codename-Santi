/*******************************************************************
*
*  DESCRIPTION: class Transducer (ParallelVersion)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*		Alejandro Troccoli v2
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 29/1/2001
*
*******************************************************************/

/** include files **/
#include "transduc.h"   // base class
#include "message.h"    // class ExternalMessage 
#include "parsimu.h"    // class ParallelMainSimulator

using namespace std;

/*******************************************************************
* CLASS TransducerState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
TransducerState& TransducerState::operator=(TransducerState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->procCount = thisState.procCount;
	this->cpuLoad = thisState.cpuLoad;
	this->unsolved = thisState.unsolved;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void TransducerState::copyState(TransducerState *rhs) {
	*this = *((TransducerState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  TransducerState::getSize() const {
	return sizeof( TransducerState );
}

/*******************************************************************
* CLASS Transducer
*********************************************************************/

/*******************************************************************
* Function Name: Transducer
* Description: Constructor
********************************************************************/
Transducer::Transducer( const string &name )
	: Atomic( name ) 
	, arrived( addInputPort( "arrived" ) )
	, solved( addInputPort( "solved" ) )
	, throughput( addOutputPort( "throughput" ) )
	, cpuUsage( addOutputPort( "cpuusage" ) )
{
	frec = "0:1:0:0" ;
	tUnit = "0:1:0:0" ;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "frecuence" ) )
		frec = ParallelMainSimulator::Instance().getParameter( description(), "frecuence" ) ;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "timeUnit" ) )
		tUnit = ParallelMainSimulator::Instance().getParameter( description(), "timeUnit" ) ;
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Transducer::initFunction()
{
	procCount( 0 );
	cpuLoad(  0 );
	unsolved().erase(unsolved().begin(), unsolved().end());
	holdIn( AtomicState::active, frecuence() ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &Transducer::externalFunction( const ExternalMessage &msg )
{

#ifdef DEVS_DELAY_EXTERNAL
	string a;
	for (int i = 1; i < 100000; i++ ) {
		if ( a.length() > 100 )
			a = "";
		else
			a += i;
	}
#endif	
	cpuLoad( cpuLoad() + static_cast< long >( ( msg.time() - lastChange() ).asMsecs() * unsolved().size() ));

	Real val = Real::from_value(msg.value());
	if( msg.port() == arrived )
	{
		if( unsolved().find( val.value() ) != unsolved().end() )
		{
			MException e( string("Unresolved Work Id: ") + val.value() + " is duplicated." );
			e.addLocation( MEXCEPTION_LOCATION() );
			throw e;
		}

		unsolved()[ val.value() ] = msg.time() ;
	}

	if( msg.port() == solved )
	{
		TransducerState::JobsList::iterator cursor( unsolved().find( val.value() ) ) ;

		if( cursor == unsolved().end() )
		{
			MException e( string("Resolved Work Id: ") + val.value() + " Not Found!" );
			e.addLocation( MEXCEPTION_LOCATION() );
			throw e;
		}

		procCount( procCount() + 1) ;
		unsolved().erase( cursor ) ;
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Transducer::internalFunction( const InternalMessage &)
{
#ifdef DEVS_DELAY_INTERNAL
	string a;
	for (int i = 1; i < 100000; i++ ) {
		if ( a.length() > 100 )
			a = "";
		else
			a += i;
	}
#endif
	holdIn( AtomicState::active, frecuence() );
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Transducer::outputFunction( const CollectMessage &msg )
{
	float time( msg.time().asMsecs() / timeUnit().asMsecs() ) ;

	sendOutput( msg.time(), throughput, procCount() / time ) ;

	cpuLoad ( cpuLoad() + static_cast< long >( ( msg.time() - lastChange() ).asMsecs() * unsolved().size() )) ;
	
	sendOutput( msg.time(), cpuUsage, cpuLoad() / msg.time().asMsecs());

	return *this ;
}
