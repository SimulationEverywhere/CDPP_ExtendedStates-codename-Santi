/*******************************************************************
*
*  DESCRIPTION: exceptions definition
*               class InvalidModelIdException
*               class InvalidMessageException
*               class MException (base class)
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 27/2/1999 (v2)
*
*******************************************************************/

#ifndef _MEXCEPT_HPP
#define _MEXCEPT_HPP

/** include files **/
#include <list>
#include <iostream>
#include <cstdlib>
#include "stringp.h"     // operator +( const std::string &, int ) ;

/** definitions **/
#define MEXCEPTION_LOCATION() ( std::string("File: ") + __FILE__ + " - Method: " + __FUNCTION__ + " - Line: " + __LINE__ )

#define MASSERT(lexp) if( !( lexp ) ) { AssertException e( "Invalid assertion" ) ; e.addText( #lexp ) ; e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ; }
#define MASSERTMSG(lexp,text) if( !( lexp ) ) { AssertException e( "Invalid assertion" ) ; e.addText( #lexp ) ; e.addText( text ) ; e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ; }

#define MASSERT_ERR(str)  { std::cout << str; exit(-1); }

#define MTHROW( e ) e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ;

class MException
{
public:
	// ** Constructors ** //
	MException( const std::string &description = "" )
	: descr( description )
	{}

	MException( const MException & ) ;
	virtual ~MException() ;

	const std::string &description() const
	{return descr;}

	MException &addText( const std::string & ) ;
	MException &addLocation( const std::string & ) ;

	typedef std::list< std::string > TextList ;
	typedef std::list< std::string > LocationList ;

	const TextList &textList() const
	{return texts;}

	const LocationList &locationList() const
	{return locations;}

	std::ostream &print( std::ostream & ) const ;

	virtual const std::string type() const
	{return "MException";}

private:
	std::string descr ;
	TextList texts ;
	LocationList locations ;

};	// MException

class AssertException: public MException
{
public:
	// ** Constructors ** //
	AssertException( const std::string &description = ""): MException( description )
	{};

	const std::string type() const
	{return "AssertException";}
};	// AssertException


class InvalidMessageException: public MException
{
public:
	const std::string type() const
	{return "InvalidMessageException";}
};	// InvalidMessageException


class InvalidModelIdException: public MException
{
public:
	const std::string type() const
	{return "InvalidModelIdException";}
};	// InvalidModelIdException

class InvalidProcessorIdException: public MException
{
public:
	const std::string type() const
	{return "InvalidProcessorIdException";}
};	// InvalidProcessorIdException

class InvalidAtomicTypeException: public MException
{
public:
	const std::string type() const
	{return "InvalidAtomicTypeException";}
};	// InvalidAtomicTypeException

class InvalidPortRequest: public MException
{
public:
	const std::string type() const
	{return "InvalidPortRequest";}
};	// InvalidPortRequest


/********************/
/** inline methods **/
/********************/

inline
	MException::MException( const MException &me ) 
	: descr( me.description() )
	, texts( me.texts )
	, locations( me.locations )
{}

inline
	MException::~MException()
{}

inline
	MException &MException::addText( const std::string &str )
{
	texts.push_back( str ) ;
	return *this ;
}

inline
	MException &MException::addLocation( const std::string &str )
{
	locations.push_back( str ) ;
	return *this ;
}

inline
	std::ostream &operator << (std::ostream &os, const MException &e)
{
	e.print( os ) ;
	return os ;
}

#endif // _MEXCEPT_HPP
