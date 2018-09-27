/*******************************************************************
*
*  DESCRIPTION: class PIni
*
*  AUTHOR:    Alejandro Troccoli 
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 17/8/2000
*
*******************************************************************/

#ifndef _PARINI_HPP
#define _PARINI_HPP

/** include files **/
#include <list>
#include <map>
#include <iostream>  /// added by pocho
#include "machineid.h"	//type MachineId
#include "except.h"
#include "tokit.h"

/** forward declarations **/
//class std::istream;   /// commented by pocho

/** declarations **/
class PIni
{
public:
	PIni() ;

	typedef std::list< std::string > IdList;
	typedef std::map< MachineId, IdList, std::less< int > > MachineList;

	PIni &parse( std::istream & );
	PIni &parse( const std::string & );

	PIni &save( std::ostream & );
	PIni &save( const std::string & );

	const MachineList &machines() const;
	const IdList &machine( const MachineId idMachine ) const;

	PIni &addMachine( const MachineId idMachine ) ;
	PIni &addDependent( const MachineId idMachine, const std::string &dependent ) ;
	

	bool exists( const MachineId idMachine) const ;


private:

	PIni( const PIni& ) ;
	MachineList machinesList;

	enum Token
	{
		def_TK,
		id_TK
	} ;

	Token readToken( TokenIterator &, std::string & ) ;

};


class PIniException : public MException
{
protected:
	PIniException( const std::string &str = "Parallel Ini Exception" ): MException( str )
	{} ;
};


class PIniParseException : public PIniException
{
public:
	PIniParseException() : PIniException( "Parsing Error in Parallel Ini File" )
	{};
};

class PIniRequestException : public PIniException
{
public:
	PIniRequestException ( const std::string &msg ) : PIniException( "Data not found!!!\n R(etry), I(gnore), P(anic) ?") {
		this->addText ( msg);
	};
};


/** inline **/
inline
PIni::PIni()
{};

inline
const PIni::MachineList &PIni::machines() const
{
	return machinesList;
}

#endif //  _PARINI_HPP







