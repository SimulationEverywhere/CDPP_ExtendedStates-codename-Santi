/*******************************************************************
*
*  DESCRIPTION: class Parser, SingleParser
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Colon bug: Alejandro Lopez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*         mailto://aplopez@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 7/11/1998 (v2)
*  DATE: 10/6/2001 (Colon bug)
*
*******************************************************************/

#ifndef __PARSER_H
#define __PARSER_H

/** include files **/
#include <iterator>
#include <string>
#include <map>
#include "except.h"          // class MException
#include "tokit.h"           // class TokenIterator

/** foward declarations **/
class SyntaxNode ;
class RuleNode ;
class SpecNode ;

/** declarations **/

class SingleParser ;

class Parser
{
public:
	~Parser() ;

	Parser &parse( std::istream &source, bool printParserInformation, bool parseForPortIn, const std::string &elseFunction = "" ) ;

	SpecNode *specification() ;

private:
	friend class SingleParser ;
	friend int yylex() ;
	friend int yyerror( const char *s ) ;
	friend int yyparse() ;		// calls addRule

	Parser() ; // constructor

	int error( const std::string &text ) ;

	Parser &addRule( RuleNode *rule, int Stochastic ) ;
				// Add a rule. If Stochastic != 0 then
				// the rule has a function of random type.
	Parser &reset() ;
	
	void parsingForPortIn ( bool pfpi ) ;
	bool parsingForPortIn ( void ) ;

	int nextToken() ;
	int analizeToken( std::string &token, bool &consumed, std::string &text ) ;

	void setTempParsingForPortIn();
	void setOldParsingForPortIn();

	typedef std::pair< int, SyntaxNode * > ValuePair ;
	typedef std::map< std::string, ValuePair, std::less< std::string > > Dictionary ;


	TokenIterator *cursor ;
	Dictionary dict ;
	std::string token ;
	bool read ;

	SpecNode *spec ;

	bool printParserInfo ;
	bool tempOldParsingPortIn ;
	bool parsingPortIn ;

} ; // Parser

class ErrorParsingException: public MException
{
} ; // ErrorParsingException


class SingleParser
{
public:
	static Parser &Instance() ;

private:
	SingleParser() ;

	static Parser *instance ;
};


/** inline **/
inline
SpecNode *Parser::specification()
{
	return this->spec ;
}

inline
void Parser::parsingForPortIn( bool pfpi )
{
	parsingPortIn = tempOldParsingPortIn = pfpi ;
}

inline
bool Parser::parsingForPortIn()
{
	return parsingPortIn;
}

inline
void Parser::setTempParsingForPortIn()
{
	parsingPortIn = true;
}

inline
void Parser::setOldParsingForPortIn()
{
	parsingPortIn = tempOldParsingPortIn;
}

#endif // __PARSER_H
