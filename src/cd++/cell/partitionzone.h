/*******************************************************************
*
*  DESCRIPTION: class PartitionZone
*
*  AUTHOR: Alejandro Troccoli	
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*         
*
*  DATE: 12/02/2001
*
*******************************************************************/

#ifndef __PARTITIONZONE_H
#define __PARTITIONZONE_H

#include "zone.h"

class PartitionZone: public Zone {
	
public:
	PartitionZone( const std::string& zoneStr );
	
	const CellPosition &firstCell() 
	{ return first;}
	
	const CellPosition &lastCell()
	{ return last; }
	
	const nTupla& dimension()
	{ return dim;	}
	
	bool includes( const PartitionZone& );
	
protected:
	nTupla dim;
};

#endif // __PARTITIONZONE_H
