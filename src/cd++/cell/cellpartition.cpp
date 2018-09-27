/*******************************************************************
*
*  DESCRIPTION: class CellPartition
*
*  AUTHOR: Alejandro Troccoli	
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*         
*
*  DATE: 12/02/2001
*
*******************************************************************/

#include "cellpartition.h"

using namespace std;

/* Set the InvalidCellIndex to the maximum positive integer */
const unsigned long CellPartition::InvalidCellIndex(-1);


/*******************************************************************
* Method: Constructor
********************************************************************/
CellPartition::CellPartition(const nTupla& dimension, bool border)
: currentIndex(0)
	, localElements(0)
	, dim(dimension)
	, wrapped( border)
{}

/*******************************************************************
* Method: Destructor
********************************************************************/
CellPartition::~CellPartition() {

	PartitionZoneList::iterator cursor1;
	PartitionCellList::iterator cursor2;

	for( cursor1 = localCells.begin(); cursor1 != localCells.end(); cursor1++)
		delete *cursor1;

	for( cursor2 = remoteNeighbors.begin(); cursor2 != remoteNeighbors.end(); cursor2++)
		delete *cursor2;

}

/*******************************************************************
* Method: addLocalZone
* Adds a zone to the list of zones
********************************************************************/
CellPartition &CellPartition::addLocalZone( const string& strZone) {


	PartitionZoneDetails *pzone = new PartitionZoneDetails( strZone );

	MASSERTMSG( !localCellsInclude(pzone->zone.firstCell()), "Cannot add zone: " + strZone + " because it will overlap to existing zones.")

	pzone->firstIndex = currentIndex;

	currentIndex += pzone->zone.dimension().totalElements();
	localElements += pzone->zone.dimension().totalElements();

	pzone->lastIndex = currentIndex - 1;

	localCells.push_back( pzone );

	return *this;
}

/*******************************************************************
* Method: addNeighborCell
* Adds a neighbor cell to the partition. Duplicate cells will not be added
********************************************************************/

CellPartition &CellPartition::addNeighborCell( const CellPosition& cell) {

	// If the cell is already in the partition, do not add it
	if ( !localCellsInclude(cell) && !neighborCellsInclude( cell ) ) {

		PartitionCellDetails *celldetail = new PartitionCellDetails;

		celldetail->cell = cell;

		celldetail->cellIndex = currentIndex;

		remoteNeighbors.push_back ( celldetail );

		currentIndex++;
	}

	return *this;
}

/*******************************************************************
* Method: addNeighborCells
* Adds all the remote neighbors corresponding to the localcells
********************************************************************/
CellPartition &CellPartition::addNeighborCells( const CellPositionList& neighborList) {

	CellPartition::LocalCellsIterator cells;
	CellPositionList::const_iterator neighbors;
	nTupla pos;

	for( cells = begin(); !(cells == end()); cells++ ) {

		for ( neighbors = neighborList.begin(); neighbors!= neighborList.end(); neighbors++ ) {

			pos = *cells;
			pos += *neighbors;


			if ( borderWrapped() ) {
				pos.canonizar( dimension() );

				//if the cell already exists, it will not be added twice.
				//That is guaranteed by addNeighborCell
				addNeighborCell( pos );
			} else {

				if( dimension().includes( pos ) ) {
					addNeighborCell( pos );	
				}

			}//if

		} // for neighbors

	} //for cells

	return *this;
}

/*******************************************************************
* Method: localCellsInclude
* Returns true if the local cells include cell
********************************************************************/

bool CellPartition::localCellsInclude( CellPosition cell) const {

	PartitionZoneList::const_iterator cursor;

	if ( borderWrapped() )
		cell.canonizar( dimension());

	cursor = localCells.begin();
	while ( cursor != localCells.end() ) {

		if( cell.isInRegion( (*cursor)->zone.firstCell(), (*cursor)->zone.lastCell() ))
			return true;

		cursor++;
	};

	return false;
}

/*******************************************************************
* Method: neighborCellsInclude
* Returns true if the neighbor cells include cell
********************************************************************/

bool CellPartition::neighborCellsInclude( CellPosition cell) const {

	PartitionCellList::const_iterator cursor;

	if ( borderWrapped() )
		cell.canonizar( dimension());

	cursor = remoteNeighbors.begin();

	while ( cursor != remoteNeighbors.end() ) {

		if ( (*cursor)->cell == cell )
			return true;

		cursor++;
	}

	return false;
}

/*******************************************************************
* Method: cellIndex
* Returns the index of the cell within the partition
* If the cell is not in the partition, then it returns InvalidCellIndex
* An InvalidCellIndex is used by CellState to return Real::undef.
********************************************************************/

unsigned long CellPartition::cellIndex( CellPosition cell) const {

	// First, search in the local cells	
	PartitionZoneList::const_iterator cursor;

	if ( borderWrapped() )
		cell.canonizar( dimension());

	cursor = localCells.begin();
	while ( cursor != localCells.end() ) {

		if( cell.isInRegion( (*cursor)->zone.firstCell(), (*cursor)->zone.lastCell() ) )
		{

			nTupla pos =  cell;
			pos -= (*cursor)->zone.firstCell();

			return (*cursor)->firstIndex + pos.totalElements() - 1;
		}


		cursor++;
	};

	//Now search in the neighbor cells
	PartitionCellList::const_iterator cursor2;

	cursor2 = remoteNeighbors.begin();

	while ( cursor2 != remoteNeighbors.end() ) {

		if ( (*cursor2)->cell == cell ) {
			return (*cursor2)->cellIndex;
		}

		cursor2++;
	}

	return InvalidCellIndex;
}

/*******************************************************************
* Method: begin
********************************************************************/
CellPartition::LocalCellsIterator CellPartition::begin() const
{
	LocalCellsIterator it (localCells.begin(), localCells.end());	
	return it;
}
/*******************************************************************
* Method: end
********************************************************************/
CellPartition::LocalCellsIterator CellPartition::end() const
{
	LocalCellsIterator it;
	return it;
}



/*******************************************************************
* Method: operator=
********************************************************************/
CellPartition::LocalCellsIterator &CellPartition::LocalCellsIterator::operator=( const LocalCellsIterator &it ) 
{

	cursor = it.cursor;
	zones = it.zones;
	currentZone = it.currentZone;
	zoneListEnd = it.zoneListEnd;

	return *this;
}

/*******************************************************************
* Method: operator++
********************************************************************/
CellPartition::LocalCellsIterator &CellPartition::LocalCellsIterator::operator++( int )
{

	cursor++;

	if ( cursor == currentZone->zone.end() ) {
		zones++;
		if ( zones == zoneListEnd )
			currentZone = NULL;
		else
			cursor = (*zones)->zone.begin();
	}

	return *this;	
}

/*******************************************************************
* Method: Iterator
********************************************************************/

CellPartition::LocalCellsIterator::LocalCellsIterator( const CellPartition::PartitionZoneList::const_iterator &begin, const CellPartition::PartitionZoneList::const_iterator &end) {
	zones = begin;
	zoneListEnd = end;


	if ( begin != end ) {
		currentZone = *begin;
		cursor = currentZone->zone.begin();
	} else {
		currentZone = NULL;
	}
}

CellPartition::LocalCellsIterator::LocalCellsIterator( const CellPartition::PartitionZoneList::const_iterator &begin, const CellPartition::PartitionZoneList::const_iterator &end, PartitionZoneDetails *current) {
	zones = begin;
	zoneListEnd = end;
	currentZone = current;
	cursor = currentZone->zone.begin();
}


/*******************************************************************
* Method: operator==
********************************************************************/
bool CellPartition::LocalCellsIterator::operator ==( const LocalCellsIterator &it ) const {

	if ( currentZone == NULL )
		return it.currentZone == NULL;
	else
		return *cursor == *it;
}
