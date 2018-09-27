/*******************************************************************
*
*  DESCRIPTION: class CPU (processes jobs with a specified
*               distribution)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*	v2: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 01/02/2001
*
*******************************************************************/

/** include files **/
#include <math.h>        // fabs( ... )
#include "cpu.h"           // base header
#include "message.h"       // InternalMessage ....
#include "distri.h"        // class Distribution
#include "parsimu.h"      // class ParallelMainSimulator
#include "strutil.h"       // str2float( ... )

using namespace std;

/*******************************************************************
* CLASS CPUState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
CPUState& CPUState::operator=(CPUState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->pid = thisState.pid;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void CPUState::copyState(CPUState *rhs) {
	*this = *((CPUState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  CPUState::getSize() const {
	return sizeof( CPUState );
}

/*******************************************************************
* CLASS CPU
*********************************************************************/
/*******************************************************************
* Function Name: CPU
* Description: constructor
********************************************************************/
CPU::CPU( const string &name )
	: Atomic( name )
	, in( addInputPort( "in" ) )
	, out( addOutputPort( "out" ) )
{
	try
	{
		dist = Distribution::create( ParallelMainSimulator::Instance().getParameter( description(), "distribution" ) );

		MASSERT( dist );

		for ( register int i = 0 ; i < dist->varCount() ; i++ )
		{
			string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar(i) ) ) ;
			dist->setVar( i, str2float( parameter ) ) ;
		}
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
* Function Name: externalFunction
* Description: the CPU receives one job
********************************************************************/
Model &CPU::externalFunction( const ExternalMessage &msg )
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
	Real val = Real::from_value(msg.value());
	pid( static_cast< int >( val.value() )) ;

#ifndef DEVS_NOTRANDOM
	holdIn( AtomicState::active, VTime( static_cast<float>( fabs(distribution().get() ) ) ) ) ;
#else
	holdIn( AtomicState::active, VTime( 0,0,15,0 ) ) ;
#endif
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &CPU::internalFunction( const InternalMessage & )
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
	passivate();
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &CPU::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, pid() );
	return *this;
}

CPU::~CPU()
{
	delete dist;
}
