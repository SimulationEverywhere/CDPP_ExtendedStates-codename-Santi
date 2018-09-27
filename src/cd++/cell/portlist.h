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

#ifndef __PORTLIST_H
#define __PORTLIST_H

/** include files **/
#include <map>
#include <string>
#include "real.h"
#include "port.h"
#include "cellpos.h"
#include "value.h"

/** forward declarations **/

#define	DEFAULT_FUNCTION_InPort		""

typedef std::map< std::string, std::string, std::less< std::string > > PortInFunction;
typedef std::map< std::string, value_ptr, std::less< std::string > >   PortValues;
typedef std::map< PortId, Port *, std::less< PortId > > PortList;

typedef std::multimap< const std::string, const Port * > VirtualPortList;


const Port *getPort( VirtualPortList *pl, std::string name);
		// Devuelve el puerto identificado por el nombre  indicado.


/////////////////////////////////////////////////////////////////////////////
// THIS ARE THE STRUCTS FOR FlatCoupledCell
/////////////////////////////////////////////////////////////////////////////

class CellDescription
{
public:
	CellDescription() { }
	CellDescription( CellPosition &cp, std::string &ip );

	const CellPosition &cellPos() const { return cell_pos; }
	void cellPos( const CellPosition &cp ) { cell_pos = cp; }
	
	const std::string &inPort() const { return in_port; }
	void inPort( const std::string &ip ) { in_port = ip; }

private:
	CellPosition	cell_pos ;
	std::string		in_port ;
} ;


inline
CellDescription::CellDescription( CellPosition &cp, std::string &ip )
: cell_pos( cp )
, in_port( ip )
{
}

namespace std {
	template<>
	inline
	bool less<CellDescription>::operator ()(const CellDescription &a, const CellDescription &b) const
	{
		return (a.cellPos() < b.cellPos()) || (a.cellPos() == b.cellPos() && a.inPort() < b.inPort() );
	}

}

typedef std::map< CellDescription, std::string, std::less< CellDescription > > FlatPortInFunction;
	// Nota: < <cellPos, inPort>, functionName> >
	
typedef std::map< CellDescription, value_ptr, std::less< CellDescription > >   FlatPortValues;
	// Nota: < <cellPos, inPort>, lastValue> >

#endif // __PORTLIST_H
