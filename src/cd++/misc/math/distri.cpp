/*******************************************************************
*
*  DESCRIPTION: class Distribution
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
#include <ctime>
#include <cstdlib>
#include "distri.h"     // class definition
#include "strutil.h"    // lowercase
extern "C"  
{
#include "randlib.h"
}

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: create
********************************************************************/
Distribution *Distribution::create( const string &distributionName )
{
	string name = lowerCase( distributionName );

	if( name == "chi" )
		return new( ChiDistribution );

	if( name == "constant" )
		return new( ConstantDistribution ) ;

	if( name == "normal" )
		return new( NormalDistribution ) ;

	if( name == "exponential" )
		return new( ExponentialDistribution ) ;

	if( name == "poisson" )
		return new( PoissonDistribution ) ;

	InvalidDistribution e;
	e.addText( "Invalid Distribution Type: " + distributionName ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: Distribution
* Description: Constructor
********************************************************************/
Distribution::Distribution() 
{
	time_t seconds;
	time( &seconds );
	srand( seconds );
	string str ;
	char c ;

	for ( register int i = 1 ; i <= 20 ; i++ )
	{
#ifndef DEVS_NOTRANDOM
		c = 50 + rand()%50;
#else
		//For testing purposes, it may be useful to allways have the same sequence of
		//random numbers. By using allways the same seed, this can be achieved.
		c = 50 + i;
#endif
		str += c;
	}
	long seed1;
	long seed2;

	// create the seeds
	phrtsd( const_cast< char *>( str.c_str() ), &seed1, &seed2 );
	setall( seed1, seed2 );
}

/*******************************************************************
* Function Name: getVar
********************************************************************/
const string Distribution::getVar( unsigned int varIndex ) const
{
	if ( (int) varIndex >= this->varCount() )
	{
		MException e( string("Invalid Variable Index: ") + (int)varIndex );
		MTHROW( e ) ;   
	}
	return get(varIndex);
}

/*******************************************************************
* Function Name: setVar
********************************************************************/
Distribution &Distribution::setVar( unsigned int varIndex, float value )
{
	if ( (int) varIndex >= varCount() )
	{
		MException e( string("Invalid Variable Index: ") + static_cast<int>( varIndex )  );
		MTHROW( e ) ;
	}
	return set(varIndex, value);
}

/*******************************************************************
* Function Name: set
********************************************************************/
Distribution &NormalDistribution::set( unsigned int index, float value ) 
{
	if( index == 0 )
		mean = value ;
	else
		deviation = value ;

	return *this ;
}

float ChiDistribution::get()
{
	return genchi(df);
}

float NormalDistribution::get()
{
	return gennor(mean, deviation);
}

float PoissonDistribution::get()
{
	return ignpoi( mu );
}

float ExponentialDistribution::get()
{
	return genexp( av );
}
