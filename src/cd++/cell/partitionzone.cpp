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

#include "partitionzone.h"

using namespace std;

/*******************************************************************
* Method: Constructor
********************************************************************/

PartitionZone::PartitionZone( const string& strZone) : Zone( strZone) {


	nTupla uno;
	uno.fill( first.dimension(), 1);

	dim = last;
	dim -= first;	
	dim += uno;

}

/*******************************************************************
* Method: includes
* Description: returns True if the intersection of the two zones is
* not empty (i.e. they share a cell in common)
********************************************************************/
bool PartitionZone::includes( const PartitionZone& thatZone) {
	return thatZone.last <= last;
}
