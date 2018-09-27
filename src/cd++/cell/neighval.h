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

#ifndef __NEIGHBORHOOD_VALUE_H
#define __NEIGHBORHOOD_VALUE_H

/** include files **/
#include <map>
#include <list>
#include "except.h"	// class MException
#include "cellpos.h"	// NeighborPosition
#include "coupcell.h"	// CellPositionList
#include "value.h"

/** declarations **/
class NeighborhoodValue
{
public:
	typedef std::map<std::string, value_ptr> CellPorts;
	typedef std::map<CellPosition, CellPorts, std::less < CellPosition > > NeighborList;

	NeighborhoodValue() {};
	NeighborhoodValue& create( const CoupledCell      &coupled, 
	                           const CellPositionList &neighbors, 
				   const CellPosition     &center, 
				   const std::list<std::string>     &ports);	// default constructor

	~NeighborhoodValue()		// Destructor
	{}

	// ** Modifiers ** //
	NeighborhoodValue &set( const std::string intportname, const AbstractValue &v )
	{ return set(NeighborPosition(dim,0), intportname, v);}

	NeighborhoodValue &set( const NeighborPosition& n, const std::string &port, const AbstractValue &v);

	// ** Queries ** // 
	bool isValid(CellPosition &cp) const;
			// Returns true if the CellPosition is contained
			// in the neighborhood.

	const AbstractValue &get( const NeighborPosition &n, const std::string &port ) const ; // throws InvalidNeighborRef

	const AbstractValue &get(const std::string &port) const
	{ return get( NeighborPosition(dim,0), port ); }

	unsigned dimension() const
	{return dim;}

	void print(std::ostream &os);

	CellPosition centralPosition() const
	{ return centralCell; }

	NeighborhoodValue& operator = ( const NeighborhoodValue& );

	const NeighborList& neighborValues() const
	{ return neighborList; }

private:

	NeighborList neighborList;
	unsigned	dim;
	CellPosition	centralCell;


	NeighborhoodValue( const NeighborhoodValue & ) ; // copy constructor

}; // NeighborhoodValue

class InvalidNeighbordRef : public MException
{
public:
	InvalidNeighbordRef(): MException( "Invalid Neighbor reference!" )
	{}
};

#endif // __NEIGHBORHOOD_VALUE_H
