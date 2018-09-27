/*******************************************************************
*
*  DESCRIPTION: class Zone
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  VERSION 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 21/4/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include "zone.h"    // class Zone

using namespace std;

/*******************************************************************
* Method: end
********************************************************************/
Zone::Iterator Zone::end()
{
	CellPosition	cp( this->last );
	cp -= this->first;
	Iterator it( this->first, this->last, cp );
	it++ ;

	return it;
}

/*******************************************************************
* Method: Iterator
********************************************************************/
Zone::Iterator::Iterator( const CellPosition &f, const CellPosition &l, const CellPosition &offset )
: first( f )
, last( l )
{
	MASSERTMSG( f.dimension() == l.dimension() && l.dimension() == offset.dimension(), "Attempt to create a Zone Iterator with CellPositions of different dimension");

	current = f;
	current += offset;
}

/*******************************************************************
* Method: Iterator
********************************************************************/
Zone::Iterator::Iterator( const CellPosition &f, const CellPosition &l )
: first( f )
, last( l )
{
	CellPosition	offset( f.dimension(), 0);
	current = f;
	current += offset;

	MASSERTMSG( f.dimension() == l.dimension() && l.dimension() == offset.dimension(), "Attempt to create a Zone Iterator with CellPositions of different dimension");
}

/*******************************************************************
* Method: Iterator
********************************************************************/
Zone::Iterator::Iterator( const Iterator &it )
:first( it.first )
,last( it.last )
,current( it.current )
{}

/*******************************************************************
* Method: operator=
********************************************************************/
Zone::Iterator &Zone::Iterator::operator=( const Iterator &it ) 
{
	first = it.first;
	last = it.last;
	current = it.current;

	return *this;
}

/*******************************************************************
* Method: operator++
********************************************************************/
Zone::Iterator &Zone::Iterator::operator++( int )
{
	register long	posUlt = current.dimension() - 1;
	register long	cant;
	register bool	incremento = true;

	while (posUlt >= 0 && incremento)
	{
		if ( (cant = current.get(posUlt) + 1) > last.get( posUlt ) )
		{
			current.setElement( posUlt, first.get(posUlt) );
			posUlt--;
		}
		else
		{
			current.setElement( posUlt, cant );
			incremento = false;
		}
	}
	if (posUlt < 0)
		current.setElement( 0, last.get(0) + 1 );

	return *this;
}

/*******************************************************************
* Method: createFrom
********************************************************************/
Zone & Zone::createFrom( const string & zoneStr ) 
{
	register int pp( zoneStr.find( ".." ) );

	try
	{
		if( pp < 0 )
			last = first = CellPosition( zoneStr );
		else
		{
			first = CellPosition( zoneStr.substr( 0, pp ) );
			last = CellPosition( zoneStr.substr( pp+2 ) );
		}
	} catch( MException& e )
	{
		e.addText( string("Invalid String Zone ") + zoneStr );
		MTHROW(e);
	}

	CellPosition rf( first );

	rf.minCoordToCoord( first, last );
	first = rf;

	rf.maxCoordToCoord( first, last );
	last = rf;

	return *this;
}
