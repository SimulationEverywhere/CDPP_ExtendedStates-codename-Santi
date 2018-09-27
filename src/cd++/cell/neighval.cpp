/*******************************************************************
*
*  DESCRIPTION: class NeighborhoodValue
*
*  AUTHOR:    Amir Barylko, Jorge Beyoglonian
*  Version 2: Daniel Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 02/06/1999 (v2)
*  DATE: 22/02/2001 (v3)
*
*******************************************************************/

// ** include files **//
#include "neighval.h"		// base header
#include "atomcell.h"		// class AtomicCell
#include "strutil.h"            // for lowerCase()

using namespace std;

// ** public ** //

/*******************************************************************
* Method: NeighborhoodValue
* Description: operator =
********************************************************************/
NeighborhoodValue &NeighborhoodValue::operator = ( const NeighborhoodValue& other)
{
	neighborList = other.neighborList;
	centralCell = other.centralCell;
	dim = other.dim;

	return *this;
}

/*******************************************************************
* Method: NeighborhoodValue
* Description: set
********************************************************************/
NeighborhoodValue &NeighborhoodValue::set( const NeighborPosition& npos, const string &port, const AbstractValue &v)
{
	NeighborList::iterator cursor;
	string                 lower_port(lowerCase(port));

	cursor = neighborList.find(npos);
	if( cursor == neighborList.end()) {
		// Si no lo encontre, entonces npos es invalido
		InvalidNeighbordRef e ;
		e.addText(string("Invalid neighbor reference: ") + npos.print());
		MTHROW( e ) ;
	}
	
	CellPorts           &cp = cursor->second;
	CellPorts::iterator  cursor2 = cp.find(lower_port);
	
	if (cursor2 == cp.end()) {
		// Si no lo encontre, entonces port es invalido
		InvalidNeighbordRef e ;
		e.addText(string("Invalid port reference: ") + port);
		MTHROW( e ) ;
	}

	cp[lower_port] = AbstractValue::to_value_ptr(v);

	return *this;
}


/*******************************************************************
* Method: NeighborhoodValue
* Description: Constructor
********************************************************************/
NeighborhoodValue &NeighborhoodValue::create( const CoupledCell      &coupled, 
                                              const CellPositionList &neighbors,
					      const CellPosition     &center,
					      const list<string>     &ports) 
{
	CellPositionList::const_iterator cursor;
	list<string>::const_iterator     cursor2;
	
	dim = coupled.dimension().dimension();

	centralCell = center;

	// Create the neighborhood with initial undef values

	for (cursor = neighbors.begin(); cursor != neighbors.end(); cursor++ ) {
		CellPorts values;
		// XXX
		values[ lowerCase(AtomicCell::neighborChangePort) ] = AbstractValue::to_value_ptr(Real());
		for (cursor2 = ports.begin(); cursor2 != ports.end(); cursor2++)
			values[lowerCase(AtomicCell::NCInPrefix + *cursor2)] = AbstractValue::to_value_ptr(Real());
			
		neighborList[*cursor] = values;
	}

	return *this;
}

/*******************************************************************
* Method: print
********************************************************************/
void NeighborhoodValue::print(ostream &os)
{
	os << "\t\t\tNeighborhood:\n";

	NeighborList::const_iterator cursor;
	CellPorts::const_iterator    cursor2;

	for ( cursor = neighborList.begin(); cursor != neighborList.end(); cursor++ ) {
		os << "\t\t\tNeighbor " << cursor->first << " = <";
		
		for( cursor2 = cursor->second.begin(); cursor2 != cursor->second.end(); cursor2++ ) {
			if (cursor2 != cursor->second.begin())
				os << ", ";
			os << *cursor2->second << " = " << *cursor2->second;
		}
		os << ">\n";
	}
}

/*******************************************************************
* Method: isValid
********************************************************************/
bool NeighborhoodValue::isValid(CellPosition &cp) const
{
	if (neighborList.find(cp) != neighborList.end() )
		return true;

	return false;
}

/*******************************************************************
* Method: get
********************************************************************/
const AbstractValue &NeighborhoodValue::get( const NeighborPosition &npos, const string &port ) const
{
	NeighborList::const_iterator cursor;
	CellPorts::const_iterator    cursor2;

	cursor = neighborList.find(npos);
	if( cursor != neighborList.end()) {
		cursor2 = cursor->second.find(lowerCase(port));
		if (cursor2 != cursor->second.end())
			return *cursor2->second;
		
		// Si no lo encontre, entonces port es invalido
		InvalidNeighbordRef e ;
		e.addText(string("Invalid port reference: ") + port);
		MTHROW( e ) ;
	}

	// Si no lo encontre, entonces npos es invalido
	InvalidNeighbordRef e ;
	e.addText(string("Invalid neighbor reference: ") + npos.print());
	MTHROW( e ) ;
}
