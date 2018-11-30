/*******************************************************************
*
*  DESCRIPTION: struct Event
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 11/10/1998 (v2)
*
*******************************************************************/

#ifndef __EVENT_H
#define __EVENT_H

/** include files **/
#include "VTime.h"    // class VTime
#include "real.h"	// class Real
#include "port.h"       // class Port
#include "value.h"

/** foward declarations **/

/** definitions **/
struct Event
{
	VTime time ;
	const Port *port ;
	value_ptr value ;

	Event( const VTime = VTime::Zero, const Port * = NULL, const AbstractValue & = Real(0) );

	bool operator < ( const Event &ev ) const
		{return time < ev.time;}
		
	Event &operator =( const Event &ev ) ;

	const std::string asString() const
		{return time.asString() + " / " + port->asString() + " / " + value->asString();}
} ;


/** inline methods **/
inline
Event::Event( const VTime t, const Port *p , const AbstractValue &v)
: time( t )
, port( p )
, value( AbstractValue::to_value_ptr(v) )
{}

inline
Event &Event::operator =( const Event &ev )
{
	time = ev.time ;
	port = ev.port ;
	value = ev.value ;
	return *this;
}

#endif   //__EVENT_H
