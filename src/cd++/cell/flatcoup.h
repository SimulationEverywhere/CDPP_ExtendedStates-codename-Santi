/*******************************************************************
*
*  DESCRIPTION: class FlatCoupledCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 24/04/1999 (v2)
*
*******************************************************************/

#ifndef __FLAT_CELL_COUPLED_H
#define __FLAT_CELL_COUPLED_H

/** include files **/
#include <list>
#include <map>
#include <iostream>
#include "coupcell.h"   // class CoupledCell
#include "real.h"	// The state of a cell
#include "cellpos.h"    // class CellPosition
#include "VTime.h"       // class VTime
#include "port.h"       // class Port
#include "portlist.h"
#include "impresion.h"

/** foward declarations **/
class FlatCoordinator ;

#define	FLAT_COUPLED_CELL_NAME	"FlatCoupledCell"

/** declarations **/
class FlatCoupledCell: public CoupledCell
{
public:
	~FlatCoupledCell()
	{ delete select; delete xList; delete yList; }

	CoupledCell &createCells( const CellPositionList &neighbors, const CellPositionList &selectList ) ;

	CoupledCell &setCellValue( const CellPosition &, const AbstractValue & ) ;

	CoupledCell &setLocalTransition( const CellPosition &, const LocalTransAdmin::Function & ) ;

	void setPortInFunction( const CellPosition &cellPos, const std::string &sourcePort, const std::string &actionName );

	Model &initFunction();

	Model &externalFunction( const VTime&, const CellPosition&, bool = false, Real mtv = Real::tundef, const std::string &portIn = "" );

	Model &internalFunction( const VTime& );

	std::string className() const
	{return FLAT_COUPLED_CELL_NAME;}

protected:
	Model &addInfluence( const std::string &sourceName, const std::string &sourcePort, const std::string &destName, const std::string &destPort) ;

	void setLastValuePortIn( const CellPosition &cellPos, const std::string &portIn, const AbstractValue &value);

private:

	friend class ParallelModelAdmin;
	friend class ParallelMainSimulator ;
	friend class FlatCoordinator ;

	// ** Types ** //
	struct NextEvent
	{
		VTime time ;
		CellPosition pos ;
		value_ptr value ;
	} ; // NextEvent

	friend std::ostream &operator <<( std::ostream &os, NextEvent &next ) ;

	typedef std::list< NextEvent > NextEventList ;
	typedef std::map< CellPosition, LocalTransAdmin::Function > FunctionZones ;

	struct ElementList
	{
		CellPosition	cellPosition;
		std::string		port;
	}; // ElementList

	// ** Instance variables ** //
	NextEventList eventList ;
	FunctionZones funcZones ;
	CellPositionList neighbors ;

	long *select ;

	VirtualPortList *xList;
	VirtualPortList *yList;

	FlatPortValues		portValues;
	FlatPortInFunction	portFunction;

	// ** Methods ** //
	FlatPortValues &getPortValues()
	{return portValues;}

	FlatPortInFunction &getPortFunction()
	{return portFunction;}

	const std::string &getPortInFunction( const CellPosition &cellPos, const std::string &sourcePort );

	void getInputPortValues( PortValues *pv, const CellPosition &cellPos, const std::string &portIn );

	void getOutputPorts( VirtualPortList **vpl, const CellPosition &cellPos );

	FlatCoupledCell( const std::string &name )
		: CoupledCell( name )
	{}

	VirtualPortList *externalList()
	{return xList;}

	NextEventList &nextEventList()
	{return eventList;}

	FlatCoupledCell &insertByTime( const VTime &t, const CellPosition &pos, const AbstractValue &v ) ;

} ; // FlatCoupledCell

/** inline **/
inline
	std::ostream &operator <<( std::ostream &os, FlatCoupledCell::NextEvent &next )
{
	os << next.time.asString() << " |" << next.pos << "| " << next.value->asString(Impresion::Default.Width(), Impresion::Default.Precision() ) ;
	return os ;
}

#endif // __FLAT_CELL_COUPLED_H
