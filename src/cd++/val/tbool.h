/*******************************************************************
*
*  DESCRIPTION: class TValBool (a boolean with three posible values)
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 11/04/1999 (v2)
*
*******************************************************************/

#ifndef __TRIVALENT_BOOL_H
#define __TRIVALENT_BOOL_H

/** include files **/
#include "except.h"
#include "real.h"
#include "evaldeb.h"

/** foward declarations **/
#define	T_NOT_TYPE	"NOT"
#define	T_XOR_TYPE	"XOR"
#define	T_IMP_TYPE	"IMP"
#define	T_EQV_TYPE	"EQV"
#define	T_AND_TYPE	"AND"
#define	T_OR_TYPE	"OR"

class Real;

/** declarations **/
class TValBool
{
public:
	TValBool() ;			// default constructor
	explicit TValBool( bool ) ;	// constructor
	explicit TValBool( char ) ; 
	TValBool( int ) ;             
	TValBool( const TValBool & ) ; //copy constructor

	operator int() const ;		// casting
	operator Real() const;		// casting

	TValBool operator && ( const TValBool & ) const ;
	TValBool operator || ( const TValBool & ) const ;
	TValBool operator !() const  ;

	TValBool &operator =( const TValBool & ) ;
	TValBool &operator &= ( const TValBool & )  ;
	TValBool &operator |= ( const TValBool & )  ;

	bool operator == ( const TValBool & ) const ;

	char asChar() const;
	TValBool Xor( const TValBool & ) const;
	TValBool imp( const TValBool & ) const;
	TValBool eqv( const TValBool & ) const;

	static const TValBool ttrue  ;
	static const TValBool tfalse ;
	static const TValBool tundef ;

protected:
	typedef int Value ;
	TValBool::Value val() const ;

private:
	TValBool::Value value ;
} ;


template <class T, class Z>
struct t_and : public std::binary_function<T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{	
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (and) ";

		return t1 && t2;
	}

	std::string type(){return T_AND_TYPE;}
};


template <class T, class Z>
struct t_or : public std::binary_function<T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{	
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (or) ";

		return t1 || t2;
	}

	std::string type(){return T_OR_TYPE;}
};


template <class T, class Z>
struct t_not : public std::unary_function<T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (not) ";

		return !t;
	}

	std::string type(){return T_NOT_TYPE;}
};


template <class T, class Z>
struct t_eqv : public std::binary_function<T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{	
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (eqv) ";

		return t1.eqv(t2);
	}

	std::string type(){return T_EQV_TYPE;}
};


template <class T, class Z>
struct t_imp : public std::binary_function<T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{	
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (imp) ";

		return t1.imp(t2);
	}

	std::string type(){return T_IMP_TYPE;}
};


template <class T, class Z>
struct t_Xor : public std::binary_function<T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{	
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (Xor) ";

		return t1.Xor(t2);
	}
	
	std::string type(){return T_XOR_TYPE;}
};


typedef t_and< TValBool, TValBool > TVB_and;
typedef t_or< TValBool, TValBool > TVB_or;
typedef t_not< TValBool, TValBool > TVB_not;
typedef t_Xor< TValBool, TValBool > TVB_Xor;
typedef t_imp< TValBool, TValBool > TVB_imp;
typedef t_eqv< TValBool, TValBool > TVB_eqv;


/** inline **/
inline
TValBool::TValBool()
: value( -1 )
{}

inline
TValBool::TValBool( int v )
{
	if( v == 0.0 || v == 1.0 || v == -1 )
		value = v ;
	else
		MASSERTMSG( false, "Invalid TValBool value from int, values are 0 (false), 1 (true), -1 (undef)" );
}

inline
TValBool::TValBool( bool b )
: value( b )
{}

inline
TValBool::TValBool( const TValBool &tvb )
: value( tvb.value )
{}


inline 
TValBool::TValBool( char c ) 
: value( -1 )
{
	if( c == '1' || c == 'T' || c == 't' )
		value = 1 ;
	else if( c == '0' || c == 'f' || c == 'F' )
		value = 0 ;
}

inline
TValBool::operator int() const
{
	return value ;
} 

inline
TValBool::Value TValBool::val() const
{
	return value;
}

inline
bool TValBool::operator == ( const TValBool &tvb ) const
{
	return value == tvb.val() ;
}

inline
TValBool TValBool::Xor( const TValBool &tvb ) const
{
	if (tvb == tundef || *this == tundef)
		return tundef;
	
	if ( (tvb == ttrue && *this == ttrue) || (tvb == tfalse && *this == tfalse) )
		return tfalse;
		
	return ttrue;
}


inline
TValBool TValBool::imp( const TValBool &tvb ) const
{
	if ( (tvb == tundef && (*this == ttrue || *this == tundef) ) ||
	     ( tvb == tfalse && *this == tundef ) )
	 	return tundef;
	 
	if (tvb == tfalse && *this == ttrue)
		return tfalse;
		
	return ttrue;
}

inline
TValBool TValBool::eqv( const TValBool &tvb ) const
{
	if (tvb == *this)
		return ttrue;

	return tfalse;
}


inline
TValBool TValBool::operator &&( const TValBool &tvb ) const
{
	if( tfalse == tvb || *this == tfalse )
		return tfalse ;

	if( tundef == tvb || *this == tundef )
		return tundef ;

	return ttrue ;
}


inline
TValBool TValBool::operator ||( const TValBool &tvb ) const
{
	if( ttrue == tvb || *this == ttrue )
		return ttrue ;

	if( tundef == tvb || *this == tundef )
		return tundef ;

	return tfalse ;
}

inline
TValBool TValBool::operator !() const
{
	if( *this == tundef )
		return tundef ;

	return TValBool( !value );
}

inline
TValBool &TValBool::operator =( const TValBool &tvb )
{
	value = tvb.val() ;
	return *this ;
}

inline
TValBool &TValBool::operator &=( const TValBool &tvb )
{
	(*this) = this->operator &&( tvb ) ;
	return *this ;
}

inline
TValBool &TValBool::operator |=( const TValBool &tvb )
{
	(*this) = this->operator ||( tvb ) ;
	return *this ;
}

inline
char TValBool::asChar() const
{
	switch( val() )
	{
		case 1:  return 'T';
		case 0:  return 'F';
	}
	return '?';
}

#endif // __TRIVALENT_BOOL_H
