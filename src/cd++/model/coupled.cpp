/*******************************************************************
*
*  DESCRIPTION: class Coupled
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  VERSION 2: Daniel Rodriguez]
*  VERSION 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atrocol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 27/2/1999 (v2)
*  DATE: 9/1/2000  (v3)
*
*******************************************************************/

/** include files **/
#include <algorithm>
#include "coupled.h"    // base header
#include "pmodeladm.h"	// SingleParallelModelAdm

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: addModel
* Description: add a model to the list of children
********************************************************************/
Model &Coupled::addModel( Model &mod )
{
	mod.parent( this );

	//These two lines might be obsolete and could be removed.
	ModelList::iterator cursor = find_if( childs.begin(), childs.end(), bind2nd( greater<ModelId>(),  mod.id() ) );
	childs.insert( cursor, mod.id() );

	//Add a new model. The insertion must be ordered by ModelId.
	//When creating cellular models, dependants are not added in the same order
	//they are created and the ModelId sorting serves to keep the select
	//clause working. If the DEVS select clause becomes obsolete, ordered
	//insertion can be removed.

	PModelList::iterator cursor2;
	for ( cursor2 = pchilds.begin(); 
	cursor2 != pchilds.end() && (*cursor2)->id() < mod.id(); 
	cursor2++ );

	pchilds.insert( cursor2, &mod );

	return *this;
}

/*******************************************************************
* Function Name: addInfluence
* Description: add a link between two ports
********************************************************************/

Model &Coupled::addInfluence( const string &sourceName, const string &sourcePort, const string &destName, const string &destPort) 
{
	Port &port1( SingleParallelModelAdm::Instance().model( sourceName ).port( sourcePort ) );
	Port &port2( SingleParallelModelAdm::Instance().model( destName ).port( destPort ) );

	port1.addInfluence( port2 );
	return *this;
}

/*******************************************************************
* Function Name: totalProcCount
********************************************************************/
unsigned long Coupled::totalProcCount() const
{
	unsigned long count = 0;

	PModelList::const_iterator cursor;
	for ( cursor = pchilds.begin(); cursor != pchilds.end() ; cursor++ )
		count += (*cursor)->totalProcCount();

	count += modelPartition().size();

	return count;
}

/*******************************************************************
* Function Name: localProcCount
********************************************************************/
unsigned long Coupled::localProcCount() const 
{
	unsigned long count = 0;

	PModelList::const_iterator cursor;
	for ( cursor = pchilds.begin(); cursor != pchilds.end() ; cursor++ )
		count += (*cursor)->localProcCount();

	if ( localProc() != ParallelProcessor::InvalidId )
		count++;

	return count;
}
