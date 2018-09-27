/*******************************************************************
*
*  DESCRIPTION: class TValBool ( constants )
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 02/3/1999 (v2)
*
*******************************************************************/

/** include files **/
#include "tbool.h"

using namespace std;

/** public data **/
const TValBool TValBool::ttrue( true )  ;
const TValBool TValBool::tfalse( false ) ;
const TValBool TValBool::tundef( -1 ) ;

/** public functions **/
TValBool::operator Real() const
{
	return value == -1 ? Real() : Real(value);
}
