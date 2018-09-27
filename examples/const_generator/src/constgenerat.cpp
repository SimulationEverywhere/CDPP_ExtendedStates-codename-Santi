/*******************************************************************
*
*  DESCRIPTION: class Generator
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
#include "constgenerat.h"       // base header
#include "message.h"       // class InternalMessage 
#include "parsimu.h"      // class Simulator
#include "strutil.h"       // str2Value( ... )

using namespace std;

/*******************************************************************
* Function Name: Generator
* Description: constructor
********************************************************************/
ConstGenerator::ConstGenerator( const string &name )
	: Atomic( name ),
	  stop(addInputPort("stop")),
	  out( addOutputPort( "out" ) )
{

	string time( ParallelMainSimulator::Instance().getParameter( description(), "frequency" ));

	if( time != "" ) {
		preparationTime = time;
	}	
	else
	{
		MException e;
		e.addText( "No frequency parameter has been found for the model " + description() );
		MTHROW ( e );
	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &ConstGenerator::initFunction()
{
	holdIn( AtomicState::active, preparationTime ) ;
	return *this ;
}

Model &ConstGenerator::externalFunction(const ExternalMessage &msg)
{
	if(msg.port() == stop)
		passivate();

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &ConstGenerator::internalFunction( const InternalMessage & )
{
	holdIn( AtomicState::active, preparationTime) ;
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &ConstGenerator::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, 0. ) ;
	return *this ;
}

