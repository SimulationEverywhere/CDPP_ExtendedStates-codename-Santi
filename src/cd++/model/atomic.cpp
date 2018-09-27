/*******************************************************************
*
*  DESCRIPTION: class Atomic
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 25/4/1999
*  DATE: 07/11/2000
*
*******************************************************************/

/** include files **/
#include "atomic.h"             // base header
#include "message.h"
#include "except.h"

using namespace std;

/*******************************************************************
* Function Name: Destructor
********************************************************************/
Atomic::~Atomic()	// Destructor
{}

/*******************************************************************
* Function Name: Constructor
********************************************************************/
Atomic::Atomic( const string &name )	// Constructor
: Model( name )
{}

/*******************************************************************
* Function Name: holdIn
* Description: Changes the atomic state and the time left to the next change 
********************************************************************/
Model &Atomic::holdIn( const AtomicState::State &s, const VTime &t )
{
	state( s );
	nextChange( t );
	return *this;
}

/*******************************************************************
* Function Name: passivate
* Description: Sets thes passive state and the next change to infinity 
********************************************************************/
Model &Atomic::passivate()
{
	state( AtomicState::passive );
	nextChange( VTime::Inf );
	return *this;
}

/*******************************************************************
* Function Name: allocateState
* Description: Creates a new instance of the state object 
********************************************************************/
ModelState* 
Atomic::allocateState() {
	return new AtomicState;
}

/*******************************************************************
* Function Name: externalFunction
* This function is provided for backward compatibility between DEVS
* and parallel DEVS code. Please, do not make abstract since it will
* require models to define this function which is obsolete for 
* Parallel DEVS
********************************************************************/
Model &Atomic::externalFunction( const ExternalMessage & )
{
	MException e ;
	e.addText( "Can not call Atomic::externalFunction( const ExternalMessage&)!" ) ;
	MTHROW( e ) ;

}
/*******************************************************************
* Function Name: externalFunction
* Description: The default definition allows to maintain backward compatibility
* it can only be used for external messages containing a real value!
********************************************************************/
Model &Atomic::externalFunction ( const MessageBag & msgs ) 
{

	MessageBag::iterator cursor;

	for( cursor = msgs.begin(); !(cursor == msgs.end()); cursor++ )
	{
		externalFunction( *(( ExternalMessage* )( *cursor )) );
	}

	return *this;
}

/*******************************************************************
* Function Name: confluentFunction
* Description: Returns the number of local processors for the model
********************************************************************/
Model &Atomic::confluentFunction ( const InternalMessage &intMsg, const MessageBag &extMsgs )
{
	//Default behavior for confluent function:
	//Proceed with the internal transition and the with the external
	internalFunction( intMsg );

	//Set the elapsed time to 0
	lastChange( intMsg.time() );

	//Call the external function
	externalFunction( extMsgs );

	return *this;

}


/*******************************************************************
* Function Name: localProcCount
* Description: Returns the number of local processors for the model
********************************************************************/
unsigned long Atomic::localProcCount() const {

	if ( localProc() != ParallelProcessor::InvalidId ) 
		return 1;
	else
		return 0;
}

/*******************************************************************
* Function Name: totalProcCount
* Description: Returns the number of total processors for the model
********************************************************************/
unsigned long Atomic::totalProcCount() const
{
	return 1;
}

/*******************************************************************
* Function Name: getCurrentState
********************************************************************/
ModelState* Atomic::getCurrentState() const
{ return ((ParallelSimulatorState* )processor().state->current)->modelState;}

/*******************************************************************
* Function Name: getCurrentState
********************************************************************/
ModelState* Atomic::getCurrentState() 
{ return ((ParallelSimulatorState* )processor().state->current)->modelState;}

/*******************************************************************
* Function Name: createParallelProcessor
********************************************************************/
ParallelProcessor &Atomic::createParallelProcessor()
{ return SingleParallelProcessorAdmin::Instance().generateProcessor(this, localProc());}

double Atomic::get_optional_double_param(
		const std::string &param_name,
		double default_value)
{
	double param_value;

	try
	{
		std::stringstream param_str(ParallelMainSimulator::Instance().getParameter(this->description(), param_name));
		param_str >> param_value;
	}
	catch(MException &e)
	{
		param_value = default_value;
	}

	return param_value;
}

double Atomic::get_mandatory_double_param(
		const std::string &param_name)
{
	double param_value;

	std::stringstream param_str(ParallelMainSimulator::Instance().getParameter(this->description(), param_name));
	param_str >> param_value;

	return param_value;
}
