/*******************************************************************
*
*  DESCRIPTION: class VTime
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*          Alejandro Troccoli (v2)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*	 04/11/2000	(v2)
*
*******************************************************************/

#ifndef __VTIME_H__
#define __VTIME_H__

/** include files **/
#include <string> // class std::string

/** definitions **/
typedef int Hours ;
typedef int Minutes ;
typedef int Seconds ;
typedef int MSeconds ;

#define PINFINITY VTime::Inf
#define INVALIDTIME VTime::InvalidTime
#define ZERO VTime::Zero

class VTime
{
public:
	explicit VTime( Hours h = 0, Minutes m = 0, Seconds s = 0, MSeconds ms = 0, double remaining = 0. )
		: hour( h )
		, min( m )
		, sec( s )
		, msec( ms )
		, rem(remaining)
		{ normalize(); }

	explicit VTime( const std::string &t )
		{ makeFrom(t); }
		
	explicit VTime( double seconds )
		: hour( 0 )
		, min( 0 )
		, sec( 0 )
		, msec( 0 )
		, rem( 0.0 )
		{ makeFrom( seconds ); }
	
	VTime( const VTime & ) ; // copy constructor

	// ** Modifiers ** //
	VTime &hours( const Hours & ) ;
	VTime &minutes( const Minutes & ) ;
	VTime &seconds( const Seconds & ) ;
	VTime &mseconds( const MSeconds & ) ;
	VTime &remaining( const double & );

	// ** Queries ** //
private:
	const Hours    &hours() const ;
	const Minutes  &minutes() const ;
	const Seconds  &seconds() const ;
	const MSeconds &mseconds() const ;
	const double   &remaining() const;
public:
	VTime operator +( const VTime & ) const ;	// addition operator
	VTime operator -( const VTime & ) const ;	// substraction operator
	VTime operator * (const int &mult) const;	// multiplication by an integer
	VTime operator / (const int &div) const;	//division by an integer
	VTime operator*(const VTime&) const;

	
	VTime &operator =( const VTime & ) ;			// assignment operator

	bool operator ==( const VTime & ) const ;	// Equality test
	bool operator !=( const VTime &t ) const
	{ return !(*this == t); }


	VTime &operator =( const std::string &t )		// assignment operator
		{makeFrom(t); return *this;}
	
	VTime &operator -=( const VTime &t ) ;
	VTime &operator +=( const VTime &t ) ;


	bool operator <( const VTime & ) const ;	// comparission operator 
	bool operator <=( const VTime & ) const ;	// comparission operator 
	bool operator >=( const VTime & ) const ;	// comparission operator
	bool operator >( const VTime &t ) const
	{ return t < *this; }

	std::string asString() const ;

	inline double asMsecs() const
		{ return remaining() + (mseconds() + seconds() * 1000 + minutes() * 60000 + hours() * 3600 * 1000); }

	inline double asSecs() const
		{ return this->asMsecs() / 1000.; }

	friend std::istream &operator >>( std::istream &is, VTime &t );

	
	static const VTime Zero ;
	static const VTime Inf  ;
	static const VTime InvalidTime;

private:
	Hours hour  ;
	Minutes min ;
	Seconds sec ;
	MSeconds msec ;
	double rem;

	VTime &makeFrom( const std::string & ) ;

	VTime &makeFrom( double seconds ) ;
	VTime &normalize() ;
	VTime &adjust( int &, int &, int ) ;
};

/** inline methods **/
inline
const Hours &VTime::hours() const
{
	return hour ;
}

inline
const Minutes &VTime::minutes() const
{
	return min ;
}

inline
const Seconds &VTime::seconds() const
{
	return sec ;
}

inline
const MSeconds &VTime::mseconds() const
{
	return msec ;
}

inline
const double &VTime::remaining() const
{
	return rem;
}

inline
VTime &VTime::hours( const Hours &h )
{
	hour = h ;
	return *this ;
}

inline
VTime &VTime::minutes( const Minutes &m )
{
	min = m ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::seconds( const Seconds &s )
{
	sec = s ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::mseconds( const MSeconds &ms )
{
	msec = ms ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::remaining( const double &rema )
{
	rem = rema ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::operator -=( const VTime &t )
{
	(*this) = (*this) - t ;
	return *this ;
}

inline
VTime &VTime::operator +=( const VTime &t )
{
	(*this) = (*this) + t ;
	return *this ;
}

inline
std::ostream &operator <<( std::ostream &os, const VTime &t )
{
	os << t.asString();
	return os;
}

inline
double operator*(const VTime &t, const double &d)
{
	return t.asSecs() * d;
}

inline
double operator*(const double &d, const VTime &t)
{
	return t.asSecs() * d;
}

inline
double operator+(const VTime &t, const double &d)
{
	return t.asSecs() + d;
}

inline
double operator+(const double &d, const VTime &t)
{
	return t.asSecs() + d;
}

inline
double operator-(const VTime &t, const double &d)
{
	return t.asSecs() - d;
}

inline
double operator-(const double &d, const VTime &t)
{
	return t.asSecs() - d;
}

inline
double operator/(const VTime &t, const double &d)
{
	return t.asSecs() / d;
}

inline
double operator/(const double &d, const VTime &t)
{
	return t.asSecs() / d;
}

inline
bool operator<(const VTime &t, const double &d)
{
	return t.asSecs() < d;
}

inline
bool operator<=(const VTime &t, const double &d)
{
	return t.asSecs() <= d;
}

inline
bool operator>(const VTime &t, const double &d)
{
	return t.asSecs() > d;
}

inline
bool operator>=(const VTime &t, const double &d)
{
	return t.asSecs() >= d;
}


#endif // __VTIME_H
