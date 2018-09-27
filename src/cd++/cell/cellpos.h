/*******************************************************************
*
*  DESCRIPTION: class CellPosition, class NeighborPosition
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 21/4/1999 (v2)
*
*******************************************************************/

#ifndef __CELL_POSITION_H
#define __CELL_POSITION_H

/** include files **/
#include <list>
#include "ntupla.h"

typedef nTupla NeighborPosition ;
typedef nTupla CellPosition ;
typedef std::list< CellPosition > CellPositionList ;

#endif // __CELL_POSITION_H
