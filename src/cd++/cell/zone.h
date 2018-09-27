/*******************************************************************
*
*  DESCRIPTION: class Zone
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998  
*
*******************************************************************/

#ifndef __ZONE_H
#define __ZONE_H

/** include files **/
#include "cellpos.h"
#include "except.h"

/** declarations **/

class ZoneException : public MException
{
protected:
	ZoneException( const std::string &str = "Invalid String Zone " ): MException( str )
	{} ;
};	// ZoneException


class Zone
{
public:
	Zone( const std::string &zoneStr )
	{createFrom( zoneStr );}
	
	struct Iterator
	{
		Iterator()	// default 
	{}
	
	Iterator( const CellPosition &f, const CellPosition &l, const CellPosition &off );
	Iterator( const CellPosition &f, const CellPosition &l );
	Iterator( const Iterator & ) ;
	
	Iterator &operator =( const Iterator &zoneStr );
	
	Iterator &operator ++(int) ;
	
	const CellPosition &operator *() const
	{ return current; }
	
	bool operator ==( const Iterator &it ) const
	{ return (current == *it); }

	bool operator !=( const Iterator &it ) const
	{ return !(*this == it); }

	CellPosition first, last, current ;
};

Iterator begin()
{ return Iterator( first, last ); }

Iterator end() ;

Zone &operator =( const std::string &zoneStr )
{ createFrom( zoneStr ); return *this; }

protected:

friend struct Iterator ;
Zone &createFrom( const std::string & ) ;

CellPosition first, last ;

} ; // Zone

#endif // __ZONE_H
