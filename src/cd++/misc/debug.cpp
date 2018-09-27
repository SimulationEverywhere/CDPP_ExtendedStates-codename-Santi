/*******************************************************************
*
*  DESCRIPTION: Debugging functions
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez.

*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 21/04/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include "debug.h"
#include "cellstate.h"  // class CelLState

using namespace std;

/*******************************************************************
* Method: Printinfluences
********************************************************************/
void PrintInfluences( const Port & p )
{
	cout << "Port: " << p.name() << " --> " << endl;
	
	const InfluenceList &influList( p.influences() ) ;
	
	for (InfluenceList::const_iterator cursor = influList.begin(); cursor != influList.end(); cursor++ )
		cout << "\t" << (*cursor)->modelId() << endl;
}

/*******************************************************************
* Function Name: printNeighbors
********************************************************************/
void PrintNeighbors( NeighborhoodValue &nval )
{
	for (nval.neighborList()->initCursor(); ! nval.neighborList()->endCursor(); nval.neighborList()->next() )
		cout << "\t" << nval.neighborList()->elementCell() << " = " << nval.neighborList()->elementValue()->asString( Impresion::Default.Width(), Impresion::Default.Precision() ) << "\n";
}

/*******************************************************************
* Function Name: PrintCellState
********************************************************************/
void PrintCellState( CellState &cstate )
{
	if (cstate.dimension().dimension() == 2)
		for ( register int i = 0 ; i < cstate.dimension().get(1) ; i++ )
	{
		for( register int j = 0 ; j < cstate.dimension().get(0); j++ )
		{
			nTupla	cp;
			cp.add(i,j);
			cout << ( (cstate[ cp ] == Real::tundef)==TValBool::ttrue   ?   "?" : cstate[ cp ].asString(Impresion::Default.Width(), Impresion::Default.Precision()) );
		}
		cout << endl;
	}
	else {	// The dimension > 2
		nTupla	dim( cstate.dimension() );
		
		MASSERTMSG( !dim.contains(0), "Attemp to print a CellState that contains the value 0.");
		
		CellPosition	counter( dim.dimension(), 0);
		register bool	overflow = false;
		
		while (!overflow)
		{
			cout << "\t" << counter << " = " << ((cstate[ counter ] == Real::tundef)==TValBool::ttrue   ?   "?" :  cstate[ counter ].asString(Impresion::Default.Width(), Impresion::Default.Precision()) ) << "\n";
			overflow = counter.next( dim );
		}
	}
}
