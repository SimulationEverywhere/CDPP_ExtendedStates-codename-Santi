/*******************************************************************
*
*  DESCRIPTION: 
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __PRNUTIL_H
#define __PRNUTIL_H

/** include files **/
#include <iostream>
#include <map>
#include <utility>
#include <list>
#include <iterator>

/** inline **/
template< class T >
inline
std::ostream &operator<<( std::ostream &out, const std::list< T > &l )
{
	typename std::list< T >::const_iterator cursor( l.begin() ) ;

	for( ; cursor != l.end() ; cursor ++ )
		out << *cursor << " " ;

	return out;
}

template< class T1, class T2 >
inline
std::ostream &operator <<( std::ostream &out, const std::pair<T1, T2> &p )
{
	out << "First: " << p.first << std::endl << "Second: " << p.second << std::endl ;  
	return out ;
}

template< class T1, class T2 >          
inline
std::ostream &operator<<( std::ostream &out,  const std::map< T1, T2, std::less< T1 > > &c )
{
	typename std::map< T1, T2, std::less< T1 > >::const_iterator cursor( c.begin() ) ;

	for( ; cursor != c.end() ; cursor ++ )
		out << cursor->first << " = " << cursor->second << std::endl ;

	return out;
}


#endif   //__PRNUTIL_H 
