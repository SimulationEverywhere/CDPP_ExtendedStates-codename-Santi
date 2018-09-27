/*******************************************************************
*
*  DESCRIPTION: class Ini
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*  Colon bug: Alejandro Lopez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://drodrigu@dc.uba.ar
*         mailto://aplopez@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 20/3/1999 (v2)
*  DATE: 15/6/2001 (colon bug)
*
*******************************************************************/

#ifndef _INI_HPP
#define _INI_HPP

/** include files **/
#include <list>
#include <map>
#include <iostream> 
#include "except.h"
#include "tokit.h"

/** forward declarations **/

/** declarations **/
class Ini
{
public:
	Ini() ;

	typedef std::list< std::string > IdList;
	typedef std::map< std::string, IdList, std::less< std::string > > DefList;
	typedef std::map< std::string, DefList, std::less< std::string > > GroupList;

	std::string join( IdList & );

	Ini &parse( std::istream & );
	Ini &parse( const std::string & );

	Ini &save( std::ostream & );
	Ini &save( const std::string & );

	const GroupList &groupList() const;
	const DefList &group( const std::string & ) const;
	const IdList &definition( const std::string &groupName, const std::string &defName ) const;

	Ini &addGroup( const std::string &groupName ) ;
	Ini &addDefinition( const std::string &groupName, const std::string &def ) ;
	Ini &addId( const std::string &groupName, const std::string &def, const std::string &id ) ;

	bool exists( const std::string &groupName, const std::string &defName ) const ;
	bool exists( const std::string &groupName ) const ;

private:
	Ini( const Ini& ) ;
	GroupList groups;

	enum Token
	{
		group_TK,
		def_TK,
		id_TK
	} ;

	Token readToken( TokenIterator &, std::string & ) ;
        bool isNumber( const std::string & );

    bool open_brace_seen, initial_value_seen;
};


class IniException : public MException
{
protected:
	IniException( const std::string &str = "Ini Exception" ): MException( str )
	{} ;
};


class IniParseException : public IniException
{
public:
	IniParseException() : IniException( "Parsing Error in Ini File" )
	{};
};


class IniRequestException : public IniException
{
public:
	IniRequestException( const std::string &msg ) : IniException( "Data not found!!!\nR(etry), I(gnore), P(anic)?" ) 
	{
		this->addText( msg );
	};
};

/** inline **/
inline
Ini::Ini() : open_brace_seen(false), initial_value_seen(false)
{};

inline
const Ini::GroupList &Ini::groupList() const
{
	return groups;
}

#endif //  _INI_HPP
