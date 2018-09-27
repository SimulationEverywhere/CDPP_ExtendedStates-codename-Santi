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

/** include files **/
#include "except.h"
#include "macroexp.h"

using namespace std;

ostream &MException::print( ostream &out ) const
{
	out << "Exception " << this->type() << " thrown!" << endl
	<< "Description: " << this->description() << endl ;

	if( ! locationList().empty() )
	{
		out << "Thrown in: " << endl ;
		LocationList::const_iterator lcursor ;
		for( lcursor = locationList().begin() ; lcursor != locationList().end() ; lcursor++ )
			out << "\t" << *lcursor << endl ;
	}

	if( ! textList().empty() )
	{
		out << "Description: " << endl ;
		TextList::const_iterator tcursor ;
		for( tcursor = textList().begin() ; tcursor != textList().end() ; tcursor++ )
			out << "\t" << *tcursor << endl ;
	}

	cout << "Aborting simulation...\n";
	delete &(MacroExpansion());
	return out;
}
