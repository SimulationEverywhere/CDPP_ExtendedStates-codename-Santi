/*******************************************************************
*
*  DESCRIPTION: class CellCoordinator
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

#ifndef __CELL_COORDINATOR_H
#define __CELL_COORDINATOR_H

/** include files **/
#include <map>
#include <set>
#include "coordin.h"       // base class

/** forward declarations **/
class CoupledCell ;

/** declarations **/
class CellCoordinator: public Coordinator
{
public:
	CellCoordinator( CoupledCell * );	 // Default constructor
			
	Processor &receive( const InternalMessage & ) ;

	Processor &receive( const OutputMessage & ) ;

	Processor &receive( const DoneMessage & ) ;

private:
	// ** types ** //
	typedef std::multimap< VTime, ModelId > Inminents;
	typedef std::set< ModelId > Influenced ;

	Inminents inminents ;
	Influenced influenced ;

};	// class CellCoordinator

#endif   //__CELL_COORDINATOR_H 
