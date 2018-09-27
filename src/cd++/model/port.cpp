/*******************************************************************
*
*  DESCRIPTION: class Port
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 28/2/1999 (v2)
*
*******************************************************************/

/** include files **/
#include "port.h"    // class Port

using namespace std;

/** public **/

int Port::idCount( 0 );

Port &Port::addInfluence( const Influence &influence )
{
	InfluenceList::iterator cursor ;
	
	for( cursor = influenceList.begin() ; 
	cursor != influenceList.end() && (*cursor)->modelId() < influence.modelId() ; 
	cursor++ ) ;
	
	influenceList.insert( cursor, &influence ) ;
	
	return *this ;
}

Port &Port::name( const string &n )
{
	pname = n ;
	return *this ;
}

Port::Port( const string &n, const ModelId &id )
: pid( Port::idCount ++ )
	, pname( n )
	, mid( id )
{}

int Port::newPortId()
{
	return Port::idCount++;
}
