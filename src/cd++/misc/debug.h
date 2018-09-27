/*******************************************************************
*
*  DESCRIPTION: just for debuggin purposes 
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __DEBUG_H
#define __DEBUG_H

/** include files **/
#include <iostream>
#include <list>
#include <utility>
#include "tdcell.h"

/** foward declarations **/

/** declarations **/

class NeighborhoodValue ;
class Port ;
class AtomicCell ;
class CellState ;

void PrintInfluences( const Port & );
void PrintNeighbors( NeighborhoodValue & ) ;
void PrintCellState( CellState & ) ;

/** inline **/
template <class X, class Y>
inline
ostream &operator <<( std::ostream& os, pair<X,Y> &p )
{
	os << p.first << "/" << p.second;
	return os;
}

template <class T>
inline
ostream &operator<<( std::ostream &os, std::list<T> t )
{
	for( std::list<T>::iterator cursor = t.begin(); cursor != t.end() ; os << *cursor++ << std::endl ) ;
	return os;
}

inline
void PrintList( const TransportDelayCell::Queue &q )
{
	cout << q;
}

#endif // __DEBUG_H
