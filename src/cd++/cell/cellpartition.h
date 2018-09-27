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

#ifndef __CELLPARTITION_H
#define __CELLPARTITION_H

#include <list>
#include "partitionzone.h"
#include "cellpos.h"
#include "except.h"

/** Forward declarations**/
class CellPartition;
class PartitionZoneDetails;
class PartitionCellDetails;

class CellPartitionException : public MException
{
public:
	CellPartitionException( const std::string &str = "Class CellPartition: " ): MException( str )
	{} ;
};	// PartitionZoneException


class CellPartition {

private:	

	typedef std::list< PartitionZoneDetails*> PartitionZoneList;
	typedef std::list< PartitionCellDetails*> PartitionCellList;

	PartitionZoneList localCells;
	PartitionCellList remoteNeighbors;
	unsigned long currentIndex;
	unsigned long localElements;
	nTupla dim;
	bool wrapped;

public:
	CellPartition(const nTupla& dimension, bool wrapped);
	~CellPartition();

	bool borderWrapped() const
	{ return wrapped; }

	const nTupla& dimension() const 
	{ return dim; }

	CellPartition &addLocalZone( const std::string& strZone);
	CellPartition &addNeighborCell( const CellPosition& cell);
	CellPartition &addNeighborCells( const CellPositionList& neighbors);

	bool includes( const CellPosition& cell) const;
	bool localCellsInclude( CellPosition cell) const;
	bool neighborCellsInclude( CellPosition cell) const;

	unsigned long cellIndex( CellPosition cell) const;
	unsigned long totalElements() const;
	unsigned long totalLocalElements() const;

	static const unsigned long InvalidCellIndex;

	struct LocalCellsIterator
	{
	LocalCellsIterator() : currentZone(NULL) {}	// default 
	LocalCellsIterator ( const PartitionZoneList::const_iterator &begin, const PartitionZoneList::const_iterator &end, PartitionZoneDetails *current);
	LocalCellsIterator ( const PartitionZoneList::const_iterator &begin, const PartitionZoneList::const_iterator &end);

	LocalCellsIterator &operator ++(int) ;

	const CellPosition &operator *() const
	{ return *cursor; }

	bool operator ==( const LocalCellsIterator &it ) const;
	LocalCellsIterator &operator=( const LocalCellsIterator &it );

	Zone::Iterator cursor;
	PartitionZoneList::const_iterator zones, zoneListEnd;

	PartitionZoneDetails *currentZone;
};

LocalCellsIterator begin() const;

LocalCellsIterator end() const;


};//Class CellPartition

/** Inline functions **/

inline
unsigned long CellPartition::totalElements() const {
	return currentIndex;
}

inline
unsigned long CellPartition::totalLocalElements() const {
	return localElements;	
}

class PartitionZoneDetails {

private:
	friend class CellPartition;
	friend struct CellPartition::LocalCellsIterator;

	PartitionZone zone;
	long firstIndex;
	long lastIndex;

	PartitionZoneDetails ( const std::string& strZone) : zone( strZone ){}
};

class PartitionCellDetails {
private:
	friend class CellPartition;

	CellPosition cell;
	long cellIndex;
};

/** Inline **/
inline
bool CellPartition::includes( const CellPosition& cell) const {
	return (localCellsInclude(cell) || neighborCellsInclude(cell));
}
#endif // __CELLPARTITION_H
