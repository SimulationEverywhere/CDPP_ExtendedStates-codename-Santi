/*******************************************************************
*
*  DESCRIPTION: PortList
*
*  AUTHOR:    Daniel Rodriguez.
*
*  EMAIL:     mailto://drodrigu@dc.uba.ar
*
*  DATE: 03/01/1999
*
*******************************************************************/

/** include files **/
#include "portlist.h"
#include "strutil.h"

using namespace std;

const Port *getPort( VirtualPortList *pl, string name)
{
	VirtualPortList::iterator	cursor;

	for (	cursor = pl->begin();
		cursor != pl->end() && cursor->first != name;
		cursor++ )
			;


	return  (cursor != pl->end() ? cursor->second : NULL);
}
