/*******************************************************************
*
*  DESCRIPTION: class CellState
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 22/11/1999 (v2)
*
*******************************************************************/

/** include files **/
#include <iomanip>
#include "cellstate.h"
#include "impresion.h"

using namespace std;


/** public functions **/
/*******************************************************************
* Function Name: CellState
********************************************************************/
CellState::CellState( nTupla &dim, bool wrap )
: wrapped( wrap )
{
	dimList = new nTupla;
	*dimList = dim;

	matrix = new Real*[ dim.totalElements() ];

	for (register long i = 0; i < dim.totalElements(); i++)
		 matrix[i] = new Real;
}

/*******************************************************************
* Function Name: ~CellState
********************************************************************/
CellState::~CellState()
{
	for (long i = 0; i < dimList->totalElements(); i++)
		delete matrix[i];

	delete matrix;
	delete dimList;
}

/*******************************************************************
* Function Name: operator []
********************************************************************/
const Real &CellState::operator[](CellPosition &pos) const
{
	if ( this->isWrapped() )
		pos.canonizar( *dimList );
	else if ( !dimList->includes( pos ) )
		return Real::tundef;

	return *( matrix[ pos.calculateIndex( *dimList, false ) ] );
}

/*******************************************************************
* Function Name: operator []
********************************************************************/
Real &CellState::operator[](CellPosition &pos)
{
	if ( this->isWrapped() )
		pos.canonizar( *dimList );
	else if ( ! dimList->includes( pos ) )
		return Real::tundef;

	return *( matrix[ pos.calculateIndex( *dimList, false ) ] );
}

/*******************************************************************
* Function Name: print
********************************************************************/
void CellState::print( ostream &os, char undefChar ) const
{
	if ( dimList->dimension() != 2 && dimList->dimension() != 3){
		printFormatedList( os );
		return;
	}

	// SOLO IMPRIME EL ESTADO CUANDO LA DIMENSION ES 2 o 3
	int width = Impresion::Default.Width(), prec = Impresion::Default.Precision();

	register int	tdim = 1;
	if (dimList->dimension() == 3)
		tdim = dimList->get(2);

        if (!Impresion::Default.FlatLog())
	{
		for (register int k = 0; k < tdim; k++)
		{
			os << "     " ;

			for( register int j = 0; j < dimList->get(DIM_WIDTH); j++ )
			{
				os << setw( width ) << setprecision( prec );
				if ( width < 2 )
					os << j % 10;	// Imprimo solo el ultimo digito
				else
					os << j;
			}
                        os << " ";
		}
		os << endl ;

		for ( register int k = 0; k < tdim; k++ )
		{
			os << "    +" ;
			for( register int j = 0; j < dimList->get(DIM_WIDTH) * width; j++ )
				os << "-" ;
			os << "+";
		}
		os << endl ;
	}

	int ndim = tdim;
	int basedim = 0;

	if (Impresion::Default.FlatLog())
	{
		if (Impresion::Default.FlatLogPlane() <= tdim)
		{
			ndim = Impresion::Default.FlatLogPlane();
			basedim = Impresion::Default.FlatLogPlane() - 1;
		}
		else
			ndim = 1;
	}

	for (register int i = 0; i < dimList->get(DIM_HEIGHT); i++ )
	{
		for (register int k = basedim; k < ndim; k++ )
		{
			if (!Impresion::Default.FlatLog())
				os << setw(4) << i << "|" ;

			for (register int j = 0; j < dimList->get(DIM_WIDTH); j++ )
			{
				nTupla	nt;
				nt.add(i,j);

				if (dimList->dimension() == 3)	// Si dim = 3
					nt.add(k);

				if( ((*this)[nt]).IsUndefined() )
				{
					for (register int w = 1; w < width; w++)
						os << ' ';

					os << undefChar ;
				}
				else if ( (*this)[nt] == Real::zero ){
					if ( !Impresion::Default.PrintZero() )
					{
						for (register int w = 1; w <= width; w++)
							os << ' ';
					}
					else
						os << ((*this)[nt]).asString( width, prec );
				}
				else
					os << ((*this)[nt]).asString( width, prec );
			}

			if (!Impresion::Default.FlatLog())
				os << "|";
		}
		os << endl;
	}

	if (!Impresion::Default.FlatLog())
	{
		for (register int k = 0; k < tdim; k++)
		{
			os << "    +" ;
			for (register int j = 0; j < dimList->get(DIM_WIDTH) * width; j++)
				os << "-" ;
			os << "+";
		}
		os << endl << endl; 
	}
}

/*******************************************************************
* Function Name: printFormatedList
********************************************************************/
void CellState::printFormatedList( ostream &os ) const
{
	// Esta funcion es utilizada para mostrar el estado cuando la
	// dimension es mayor a 3. Si es 2 o 3 se debe usar la fn print.
	CellPosition	counter( dimList->dimension(), 0);
	register bool	overflow = false;

	while (!overflow)
	{
		os << "\t" << counter << " = " << (*this)[counter] << "\n";

		overflow = counter.next( *dimList );
	}
	os << endl << endl;
}
