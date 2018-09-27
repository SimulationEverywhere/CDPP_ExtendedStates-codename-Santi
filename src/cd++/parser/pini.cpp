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

/** include files **/
#include <fstream>
#include "pini.h"
#include "strutil.h"  // lowerCase
#include "prnutil.h"  // operator << std::list< T >

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: machine
* Description: nombre de los atomicos asociados a una maquina
********************************************************************/
const PIni::IdList &PIni::machine( const MachineId idMachine ) const
{

	if( machinesList.find(idMachine) == machinesList.end() )
	{
		PIniRequestException e( "Machine: Not found!" ) ;
		MTHROW( e );
	}
	return machinesList.find( idMachine )->second ;
}

/*******************************************************************
* Function Name: parse
* Description: llama a parse con un ifstream
********************************************************************/
PIni &PIni::parse( const string &fileIn )
{
	ifstream in( fileIn.c_str() );
	return this->parse( in );
}

/*******************************************************************
* Function Name: parse
* Description: parsea el inifile
********************************************************************/
PIni &PIni::parse( istream & in )
{
	int currentMachine;

	TokenIterator cursor( in ) ;
	while( cursor != TokenIterator())
	{
		string word;
		Token tokenId = this-> readToken(cursor, word);

		
		switch ( tokenId ) {
			case def_TK:
				
				currentMachine = str2Int(word) ;
				machinesList[ currentMachine ];
				break;

			case id_TK:
				
				machinesList[currentMachine] . push_back(word);
				break;
		}

	}
	return *this;
}

/*******************************************************************
* Function Name: readToken
* Description: tokenize words
********************************************************************/
PIni::Token PIni::readToken( TokenIterator &cursor, string &word )
{

	word = lowerCase( *cursor );
	cursor ++ ;

	if( *cursor == ":" )
	{
		cursor ++ ;
		return def_TK ;
	} else
		return id_TK ;
}

/*******************************************************************
* Function Name: exists
********************************************************************/
bool PIni::exists( const MachineId idMachine ) const
{
	return machinesList.find( idMachine ) != machinesList.end() ;
}

/*******************************************************************
* Function Name: addGroup
* Description: add group to ini
********************************************************************/
PIni &PIni::addMachine( const MachineId idMachine )
{
	machinesList[ idMachine ] ;
	return *this ;
}


/*******************************************************************
* Function Name: save
********************************************************************/
PIni &PIni::save( const string &fileName )
{
	ofstream out( fileName.c_str() ) ;
	this->save( out ) ;
	return *this;
}

/*******************************************************************
* Function Name: save
********************************************************************/
PIni &PIni::save( ostream &out )
{
	PIni::MachineList::iterator cursor ;

	for( cursor = machinesList.begin() ; cursor != machinesList.end() ; cursor ++ )
	{
		out << '[' << cursor->first << ']' << endl ;
		out << cursor->second << endl ;   
	}
	return *this ;
}
