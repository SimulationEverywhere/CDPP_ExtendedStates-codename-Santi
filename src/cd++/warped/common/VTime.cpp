/*******************************************************************
*
*  DESCRIPTION: class VTime
/ (originally named Time in v1)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar (v2)
*
*  DATE: 27/6/1998
*  DATE: 06/11/2000 (v2)
*
*******************************************************************/

/** include files **/
#include "VTime.h"         // base class
#include "stringp.h"      // operator + int
#include <limits>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cassert>

using namespace std;

#define DEPURANDO 0

/** public data **/
const VTime VTime::Zero(0,0,0,0)	 ;	  // Zero constant
const VTime VTime::Inf( 32767, 59, 59, 999 ) ;	// Infinity constant
const VTime VTime::InvalidTime(-1,0,0,0);	//Invalid time. Must be < Zero

/*******************************************************************
* Function Name: VTime
* Description: copy constructor
********************************************************************/
VTime::VTime( const VTime &t )
	: hour( t.hours() )
	, min( t.minutes() )
	, sec( t.seconds() )
	, msec( t.mseconds() )
	, rem( t.remaining() )
{
}

/*******************************************************************
* Function Name: operator +
********************************************************************/
VTime VTime::operator +( const VTime &t ) const
{

	if ( *this == VTime::Inf || t == VTime::Inf )
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime || t == VTime::InvalidTime)
		return VTime::InvalidTime;
	
	VTime st( hours() + t.hours(),
		minutes() + t.minutes(),
		seconds() + t.seconds(),
		mseconds() + t.mseconds(),
		remaining() + t.remaining());
	

	//st.normalize();	// It's called by constructor and is not needed
	return st;
}

/*******************************************************************
* Function Name: opeartor -
********************************************************************/
VTime VTime::operator -( const VTime &t ) const
{	
	if ( *this == VTime::Inf || t == VTime::Inf )
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime || t == VTime::InvalidTime)
		return VTime::InvalidTime;
	
	if ( *this < t )
		cout << "Guarda, tiempo negativo!" << endl;

	VTime st( hours() - t.hours(),
		minutes() - t.minutes(),
		seconds() - t.seconds(),
		mseconds() - t.mseconds(),
		remaining() - t.remaining());
	
	//st.normalize();	// It's called by constructor and is not needed
	return st; 
}


/*******************************************************************
* Function Name: opeartor *
********************************************************************/
VTime VTime::operator * (const int &mult) const
{
	if (*this == VTime::Inf)
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime )
		return VTime::InvalidTime;	
	
	VTime st( hours() * mult, 
		minutes() * mult,
		seconds() * mult,
		mseconds() * mult,
		remaining() * mult);
	
	return st;
	
}

VTime VTime::operator*(const VTime& t) const
{
	return VTime(this->asSecs() * t.asSecs());
}

/*******************************************************************
* Function Name: opeartor /
********************************************************************/
VTime VTime::operator / (const int &div) const
{
	if (*this == VTime::Inf)
		return VTime::Inf;
	
	if (div == 0)
		return VTime::Inf;
	
	double total = mseconds() + 1000 * (seconds() + 60 * (minutes() + 60 * hours())) + remaining();

	double totalDivided = total / div;
	int ms = static_cast<int>(totalDivided);
	VTime st( 0,0,0, ms, totalDivided - ms);
	
	return st;
	
}
/*******************************************************************
* Function Name: opeartor ==
********************************************************************/
bool VTime::operator ==( const VTime &t ) const
{
	return ( hours()    == t.hours()    &&
		minutes()  == t.minutes()  &&
		seconds()  == t.seconds()  &&
		mseconds() == t.mseconds() &&
		std::fabs(remaining() - t.remaining()) < std::numeric_limits<double>::epsilon());

}

/*******************************************************************
* Function Name: opeartor =
********************************************************************/
VTime &VTime::operator =( const VTime &t )
{
	hour = t.hours();
	sec = t.seconds();
	min = t.minutes();
	msec = t.mseconds();
	rem  = t.remaining();
	return *this;
}

/*******************************************************************
* Function Name: operator <<
********************************************************************/
bool VTime::operator <( const VTime &t ) const
{
	return( hours() < t.hours()    ||
		( hours() == t.hours() &&
			( minutes() < t.minutes()  ||
				( minutes() == t.minutes()  &&
					( seconds() < t.seconds() ||
						( seconds() == t.seconds() &&
							(mseconds() < t.mseconds() ||
								(mseconds() == t.mseconds() &&
									remaining() < t.remaining() ) ) ) ) ) ) ) ) ;
}


bool VTime::operator <=( const VTime &t ) const
{
	return *this < t || *this == t;
}

bool VTime::operator >=( const VTime &t ) const
{
	return t < *this or *this == t;
}

/*******************************************************************
* Function Name: asString
********************************************************************/
string VTime::asString() const
{
	ostringstream sstr;
	sstr.fill('0');
	if (
		hours()    == VTime::Inf.hours()    &&
		minutes()  == VTime::Inf.minutes()  &&
		seconds()  == VTime::Inf.seconds()  &&
		mseconds() == VTime::Inf.mseconds() &&
		std::fabs(remaining() - VTime::Inf.remaining()) <
			std::numeric_limits<double>::epsilon()
		)
		sstr << "...";
	else
		sstr << setw(2) << hours() << ":"
		     << setw(2) << minutes() << ":"
		     << setw(2) << seconds() << ":"
		     << setw(3) << mseconds() << ":"
		     << remaining();
	return sstr.str();
}

/*******************************************************************
* Function Name: makeFrom
* Description: make from string
********************************************************************/
VTime &VTime::makeFrom( const string &str )
{
	if (std::count(str.begin(), str.end(), ':') > 3)
		sscanf( str.c_str(), "%d:%d:%d:%d:%lf", &hour, &min, &sec, &msec, &rem );
	else
	{
		sscanf( str.c_str(), "%d:%d:%d:%d", &hour, &min, &sec, &msec );
		rem = 0.;
	}
	normalize();

	return *this;
}

/*******************************************************************
* Function Name: makeFrom
* Description: make from seconds
********************************************************************/
VTime &VTime::makeFrom( double secs )
{
	seconds( static_cast< int >( secs ) ) ;
	secs -= hours() * 3600 + minutes() * 60 + seconds();
	double milsec = secs * 1000;
	mseconds( static_cast< int >( milsec ) ) ;
	milsec -= mseconds();
	remaining(milsec);
	normalize() ;

	return *this ;
}

/*******************************************************************
* Function Name: normalize
********************************************************************/
VTime &VTime::normalize()
{
	if (rem < 0)
	{
		int ask = static_cast<int>(-rem)+1;
		rem  += ask;
		msec -= ask;
	}

	int integerMillisecs = static_cast<int>(rem);
	rem = max(rem - integerMillisecs, 0.0);

	msec += integerMillisecs;

	// 0..999 Miliseconds
	adjust( msec, sec , 1000 );
	
	// 0..59 seconds
	adjust( sec , min , 60   );
	
	// 0..59 minutes
	adjust( min , hour, 60   );
	
	return *this ;
}

/*******************************************************************
* Function Name: adjust
********************************************************************/
VTime &VTime::adjust( int &left, int &right, int maxVal )
{
	if( left >= maxVal )
	{
		right += left / maxVal ;
		left %= maxVal ;
	} else if( left < 0 )
	{
		register int aux = abs( left / maxVal ) + ( left % maxVal == 0 ? 0 : 1 );
		right -= aux ;
		left += maxVal * aux ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: operator >>
********************************************************************/
istream &operator >>( istream &is, VTime &t )
{
	string s;
	is >> s;
	t.makeFrom(s);
}
