/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "distri.h"        // class Distribution
#include "strutil.h"

#include "ModelTemplate.h" // Cambiar nombre, base header

using namespace std;

#define VERBOSE true

#define PRINT_TIMES(f) {\
	VTime timeleft = nextChange();\
	VTime elapsed  = msg.time() - lastChange();\
	VTime sigma    = elapsed + timeleft;\
	cout << f << "@" << msg.time() <<\
		" - timeleft: " << timeleft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}

/** public functions **/

/*******************************************************************
* Function Name: [#MODEL_NAME#]
* Description: constructor
********************************************************************/
ModelTemplate::ModelTemplate( const string &name ) : 
	Atomic( name )
	// TODO: add ports here if needed (Remember to add them to the .h file also). Each in a new line.
	// Ej:
	// , out(addOutputPort( "out" ))
	// , in(addInputPort( "in" ))
{
	// TODO: add initialization code here. (reading parameters, initializing private vars, etc)
	// Code templates for reading parameters:
	// read string parameter:
	// 		stringVar = ParallelMainSimulator::Instance().getParameter( description(), "paramName" );
	// read int parameter:
	// 		intVar = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "initial" ) );
	// read time parameter:
	//		timeVar = string time( ParallelMainSimulator::Instance().getParameter( description(), "preparation" ) ) ;
	// read distribution parameters:
	//		dist = Distribution::create( ParallelMainSimulator::Instance().getParameter( description(), "distribution" ) );
	//		MASSERT( dist ) ;
	//		for ( register int i = 0; i < dist->varCount(); i++ )
	//		{
	//			string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
	//			dist->setVar( i, str2Value( parameter ) ) ;
	//		}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &ModelTemplate::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
 	this->timeLeft = VTime::Inf;
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma    = VTime::Zero; // force an internal transition in t=0;

 	// TODO: add init code here. (setting first state, etc)
 	
 	// set next transition
 	holdIn( AtomicState::active, this->sigma  ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &ModelTemplate::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dext");
#endif
	//[(!) update common variables]	
	this->sigma    = nextChange();	
	this->elapsed  = msg.time()-lastChange();	
 	this->timeLeft = this->sigma - this->elapsed; 
	
	//TODO: implement the external function here.
 	// Remember you can use the msg object (mgs.port(), msg.value()) and you should set the next TA (you might use the holdIn method).
 	// EJ:
 	// if( msg.port() == in )
	//{
	//	// Do something
	//	holdIn( AtomicState::active, this->timeLeft );
	// }
	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &ModelTemplate::internalFunction( const InternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dint");
#endif
	//TODO: implement the internal function here

	this->sigma = VTime::Inf; // stays in passive state until an external event occurs;
	holdIn( AtomicState::passive, this->sigma );
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &ModelTemplate::outputFunction( const CollectMessage &msg )
{
	//TODO: implement the output function here
	// remember you can use sendOutput(time, outputPort, value) function.
	// sendOutput( msg.time(), out, 1) ;
	// value could be a tuple with different number of elements: 
	// Tuple<Real> out_value{Real(value), 0, 1};
	// sendOutput(msg.time(), out, out_value);
	
	return *this;

}

ModelTemplate::~ModelTemplate()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
