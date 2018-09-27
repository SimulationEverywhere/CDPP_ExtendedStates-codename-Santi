/*******************************************************************
*
*  DESCRIPTION: class Generator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*	v2: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*        01/02/2001
*
*******************************************************************/

/** include files **/
#include "generat.h"       // base header
#include "message.h"       // class InternalMessage 
#include "parsimu.h"      // class Simulator
#include "distri.h"        // class Distribution 
#include "strutil.h"       // str2Value( ... )

using namespace std;

/*******************************************************************
* CLASS GeneratorState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
GeneratorState& GeneratorState::operator=(GeneratorState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->pid = thisState.pid;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void GeneratorState::copyState(GeneratorState *rhs) {
	*this = *((GeneratorState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  GeneratorState::getSize() const {
	return sizeof( GeneratorState );
}

/*******************************************************************
* CLASS Generator
*********************************************************************/

/*******************************************************************
* Function Name: Generator
* Description: constructor
********************************************************************/
Generator::Generator( const string &name )
	: Atomic( name ),
	  stop(addInputPort("stop")),
	  out( addOutputPort( "out" ) )
{
	try
	{
		dist = Distribution::create( ParallelMainSimulator::Instance().getParameter( description(), "distribution" ) );
		MASSERT( dist ) ;
		for ( register int i = 0; i < dist->varCount(); i++ )
		{
			string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
			dist->setVar( i, str2Value( parameter ) ) ;
		}

		if( ParallelMainSimulator::Instance().existsParameter( description(), "initial" ) )
			initial = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "initial" ) );
		else
			initial = 0;

		if( ParallelMainSimulator::Instance().existsParameter( description(), "increment" ) )
			increment = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "increment" ) );
		else
			increment = 1;

	} catch( InvalidDistribution &e )
	{
		e.addText( "The model " + description() + " has distribution problems!" ) ;
		e.print(cerr);
		MTHROW( e ) ;
	} catch( MException &e )
	{
		MTHROW( e ) ;
	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Generator::initFunction()
{
	pid ( initial );
	holdIn( AtomicState::active, VTime::Zero ) ;
	return *this ;
}

Model &Generator::externalFunction(const ExternalMessage &msg)
{
	if(msg.port() == stop)
		passivate();

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Generator::internalFunction( const InternalMessage & )
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

#ifndef DEVS_NOTRANDOM	
	holdIn( AtomicState::active, VTime( static_cast< float >( fabs(distribution().get() ) ) ) ) ;
#else
	holdIn( AtomicState::active, VTime( 0,0,10,0 ) ) ;
#endif
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Generator::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, pid() ) ;
	pid( pid() + increment);
	return *this ;
}

Generator::~Generator()
{
	delete dist;
}
