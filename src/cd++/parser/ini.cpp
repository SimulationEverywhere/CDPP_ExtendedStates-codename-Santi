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
*         mailto://drodrigu@dc.uba.ar
*         mailto://aplopez@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE:  4/6/1999 (v2)
*  DATE: 15/6/2001 (colon bug)
*
*******************************************************************/

/** include files **/
#include <fstream>
#include "ini.h"
#include "strutil.h"  // lowerCase
#include "prnutil.h"  // operator << std::list< T >

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: group
********************************************************************/
const Ini::DefList &Ini::group( const string &groupName ) const
{
	string lower( groupName ) ;

	if( groups.find( lower ) == groups.end() )
	{
		IniRequestException e( groupName + " Not Found!" ) ;
		MTHROW( e );
	}
	return groups.find( lower )->second;
}

/*******************************************************************
* Function Name: definition
* Description: ids asociados a la definicion
********************************************************************/
const Ini::IdList &Ini::definition( const string &groupName, const string &defName ) const
{
	const DefList &dlist( this->group( lowerCase( groupName ) ) ) ;

	if( dlist.find( lowerCase( defName ) ) == dlist.end() )
	{
		IniRequestException e( groupName + "/" + defName + " Not Found!" ) ;
		MTHROW( e );
	}
	return dlist.find( lowerCase( defName ) )->second ;
}

/*******************************************************************
* Function Name: parse
* Description: llama a parse con un ifstream
********************************************************************/
Ini &Ini::parse( const string &fileIn )
{
	ifstream in( fileIn.c_str() );
	return this->parse( in );
}

/*******************************************************************
* Function Name: parse
* Description: parsea el inifile
********************************************************************/
Ini &Ini::parse( istream & in )
{
	string actualGroup, actualDef, word;

	TokenIterator cursor( in ) ;

	while( cursor != TokenIterator() )
	{
		Token tokenId = this->readToken( cursor, word ) ;

		switch( tokenId )
		{
			case group_TK:
				actualGroup = word ;
				groups[ actualGroup ];
				break;

			case def_TK:
				actualDef = word ;
				groups[ actualGroup ][ actualDef ] ;
				break;

			case id_TK:
				groups[ actualGroup ][ actualDef ].push_back( word ) ;
				break;
		}
	}
	return *this;   
}

/*******************************************************************
* Function Name: exists
********************************************************************/
bool Ini::exists( const string &groupName ) const
{
	return groups.find( lowerCase( groupName ) ) != groups.end() ;
}

/*******************************************************************
* Function Name: exists
********************************************************************/
bool Ini::exists( const string &groupName, const string &defName ) const
{
	GroupList::const_iterator gcursor( groups.find( lowerCase( groupName ) ) ) ;

	if( gcursor == groups.end() )
		return false ;

	DefList::const_iterator dcursor( gcursor->second.find( lowerCase( defName ) ) ) ; 

	return dcursor != gcursor->second.end();
}

/*******************************************************************
* Function Name: addGroup
* Description: add group to ini
********************************************************************/
Ini &Ini::addGroup( const string &groupName )
{
	groups[ lowerCase( groupName ) ] ;
	return *this ;
}

/*******************************************************************
* Function Name: addDefinition
********************************************************************/
Ini &Ini::addDefinition( const string &groupName, const string &def )
{
	groups[ lowerCase( groupName ) ][ lowerCase( def ) ] ;
	return *this ;
}

/*******************************************************************
* Function Name: addId
********************************************************************/
Ini &Ini::addId( const string &groupName, const string &def, const string &id )
{
	groups[ lowerCase( groupName ) ][ lowerCase( def ) ].push_back( lowerCase( id ) ) ;
	return *this ;
}

/*******************************************************************
* Function Name: save
********************************************************************/
Ini &Ini::save( const string &fileName )
{
	ofstream out( fileName.c_str() ) ;
	this->save( out ) ;
	return *this;
}

/*******************************************************************
* Function Name: save
********************************************************************/
Ini &Ini::save( ostream &out )
{
	Ini::GroupList::iterator cursor ;

	for( cursor = groups.begin() ; cursor != groups.end() ; cursor ++ )
	{
		out << '[' << cursor->first << ']' << endl ;
		out << cursor->second << endl ;   
	}
	return *this ;
}

/*******************************************************************
* Function Name: readToken
* Description: tokenize words
********************************************************************/
Ini::Token Ini::readToken(TokenIterator &cursor, string &word)
{
	// To allow parsing '[' as a tuple delimiter, we verify that we are inside
	// the appropriate context (i.e., inside initialValue or inside a rule).
	if( (*cursor)[0] == '[' && !this->open_brace_seen && !this->initial_value_seen )
	{
		word = lowerCase( cursor->substr(1, cursor->size() - 2) );
		cursor++ ;
		return group_TK ;
	}

	word = lowerCase( *cursor );
	cursor ++ ;

	if(this->initial_value_seen)
		this->initial_value_seen = false;

	if(word == "initialvalue")
		this->initial_value_seen = true;

	if(word == "{")
		this->open_brace_seen = true;

	if(word == "}")
		this->open_brace_seen = false;

	if( *cursor == ":" )
	{
                // hh:mm:ss:mmm is a special case
		if ( isNumber( word ) ) {
			bool leave = false;
                        
                        while ( !leave ) {
				if ( *cursor == ":" ) {
		                        word += *cursor ;
					cursor++ ;
                                } else {
                                	leave = true;
                                }
                                
				if ( isNumber( *cursor ) ) {
		                        word += *cursor ;
					cursor++ ;
                                } else {
                                	leave = true;
                                }
                        }
			return id_TK ;
		} else {
			cursor ++ ;
			return def_TK ;
		}
	} else
		return id_TK ;
}

/*******************************************************************
* Function Name: join
********************************************************************/
string Ini::join( IdList &id )
{
	string	s("");

	for (Ini::IdList::const_iterator cursor = id.begin(); cursor != id.end(); cursor++)
		s += *cursor;
	return s;
}

/*******************************************************************
* Function Name: isNumber
* Description: checks whether the string is composed only of digits
********************************************************************/
bool Ini::isNumber( const string &str )
{
    string::const_iterator cursor = str.begin();

    while (cursor != str.end() && *cursor >= '0' && *cursor <= '9')
        cursor++;
        
    return (cursor == str.end());
}
