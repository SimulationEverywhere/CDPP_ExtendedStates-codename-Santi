/*******************************************************************
*
*  DESCRIPTION: class nTupla
*
*  AUTHOR: Daniel A. Rodriguez
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 25/04/1999 (v2)
*
*******************************************************************/

/** include files **/
#include "ntupla.h"	// class nTupla
#include "strutil.h"	// string functions
#include <cstring>

using namespace std;

/** methods **/

nTupla::nTupla( const nTupla &pos )
{
	valTupla = new int[ size = pos.dimension() ];

	// Copy the elements of Pos to ValTupla	
	for (register unsigned i = 0; i < size; i++)
		valTupla[i] = pos.valTupla[i];

	canonizada = pos.isCanonizada();
	valTotalElem = pos.totalElements();
}

nTupla::nTupla( unsigned dim, int value )
{
	valTupla = new int[ size = dim ];

	for (register unsigned i = 0; i < size; i++)
		valTupla[i] = value;

	canonizada = false;
	valTotalElem = size * value;
}

nTupla::nTupla( const string &str )
	:canonizada(false)
{
	valTupla = new int[1];
	size = 0;
	parseString(str);
	calcTotalElements();
}

void nTupla::add( int n )
{
	int		*valAux = new int[size+1];	// Backup the tupla
	register unsigned	i = 0;

	for (; i < size; i++)
		valAux[i] = valTupla[i];

	valAux[i] = n;			// Adds the element

	delete [] valTupla;
	valTupla = valAux;		// Reasigns the tupla

	canonizada = false;
	size++;
	valTotalElem *= n;
}

void nTupla::add( int n1, int n2 )
{
	int		*valAux = new int[size+2];	// Backup the tupla
	register unsigned	i = 0;

	for (; i < size; i++)
		valAux[i] = valTupla[i];

	valAux[i++] = n1;	// Adds the elements
	valAux[i] = n2;

	delete [] valTupla;
	valTupla = valAux;		// Reasigns the tupla

	canonizada = false;
	size += 2;
	valTotalElem *= n1 * n2;
}

nTupla *nTupla::addFirst( int n )
{
	int	*valAux;

	valAux = new int[++size];	// Backup the tupla

	valAux[0] = n;			// Adds the element in the front
	for (register unsigned i = 1; i < size; i++)
		valAux[i] = valTupla[i-1];

	delete [] valTupla;
	valTupla = valAux;		// Reasigns the tupla

	canonizada = false;
	valTotalElem *= n;

	return this;
}

nTupla *nTupla::addFirst( int n1, int n2 )
{
	int	*valAux;

	size += 2;
	valAux = new int[size];		// Backup the tupla

	valAux[0] = n1;			// Adds the elements in the front
	valAux[1] = n2;
	for (register unsigned i = 2; i < size; i++)
		valAux[i] = valTupla[i-2];

	delete [] valTupla;
	valTupla = valAux;		// Reasigns the tupla

	canonizada = false;
	valTotalElem *= n1 * n2;
	return this;
}

void nTupla::setElement( unsigned pos, int n )
{
	if (pos < size)
	{
		if (valTupla[pos] == 0)
		{
			valTupla[pos] = n;
			calcTotalElements();
		}
		else
		{
			valTotalElem /= valTupla[pos];
			valTotalElem *= n;
			valTupla[pos] = n;
		}
	}
	else	// Debo agregar mas elementos
	{
		int	*valAux = new int[pos+1]; // Backup the tupla
                int     prevSize = size;

		for (register unsigned i = 0; i < size; i++)
			valAux[i] = valTupla[i];

		// Adds 0 to complete
		while (size < pos){
			valAux[size++] = 0;
			valTotalElem = 0;
		}
		valAux[size++] = n;
		valTotalElem *= n;

		if (prevSize != 0)
			delete [] valTupla;
		valTupla = valAux;		// Reassigns the tupla
	}
	canonizada = false;
}

int nTupla::get( unsigned n ) const
{
	MASSERTMSG( n < size, "Attemp to get an element not contained in the tupla.");
	return valTupla[n];
}

nTupla &nTupla::canonizar( const nTupla &dim )
{
	if (canonizada)
		return *this;

	MASSERTMSG( size == dim.dimension(), "Can't standarizate two tuples with different dimensions");
	valTotalElem = 1;

	for ( register unsigned i = 0; i < size; i++)
	{
		register int dimVal = dim.valTupla[i];

		while (valTupla[i] < 0)
			valTupla[i] += dimVal;

		valTupla[i] %= dimVal;
		valTotalElem *= valTupla[i];
	}
	canonizada = true;
	return *this;
}

void nTupla::calcTotalElements()
{
	valTotalElem = 1;

	for ( register unsigned i = 0; i < size; i++ )
		valTotalElem *= valTupla[i];
}

bool nTupla::operator == ( const nTupla &t ) const
{
	if ( size != t.dimension())
		return false;

	for ( unsigned i = 0; i < size; i++ )
		if ( valTupla[i] != t.valTupla[i] )
			return false;
	return true;
}

bool nTupla::operator < ( const nTupla &t ) const
{
	if ( size != t.dimension())
		return false;

	//Compare coordinate to coordinate.
	for ( unsigned i = 0; i < size; i++)
	{
		if ( valTupla[i] > t.valTupla[i] )
			return false;

		else if ( valTupla[i] < t.valTupla[i] )
			return true; 

	}	

	return false;
}

bool nTupla::operator <= ( const nTupla &t ) const
{

	if ( size != t.dimension())
		return false;

	//Compare coordinate to coordinate.
	for ( unsigned i = 0; i < size; i++)
	{
		if ( valTupla[i] > t.valTupla[i] )
			return false;

		else if ( valTupla[i] < t.valTupla[i] )
			return true; 

	}	

	return true;

}

nTupla *nTupla::operator += ( const nTupla &t )
{
	MASSERTMSG( size == t.dimension(), "Attemp to sum tuples of different dimension");

	valTotalElem = 1;
	for (register unsigned i = 0; i < size; i++)
	{
		valTupla[i] += t.valTupla[i];
		valTotalElem *= valTupla[i];
	}
	canonizada = false;
	return this;
}

nTupla *nTupla::operator -= ( const nTupla &t )
{
	MASSERTMSG( size == t.dimension(), "Attemp to substract tuples of different dimension");

	valTotalElem = 1;
	for (register unsigned i = 0; i < size; i++)
	{
		valTupla[i] -= t.valTupla[i];
		valTotalElem *= valTupla[i];
	}
	canonizada = false;
	return this;
}

string nTupla::print() const
{
	string	s = "(";

	for (register unsigned i = 0; i < size; i++)
	{
		s += int2Str( valTupla[i] );
		if ( i != size-1 )
			s += ",";
	}
	s += ")";
	return s;
}

bool nTupla::includes( const nTupla &pos ) const
{
	MASSERTMSG( size == pos.dimension(), "Attemp to compare tuples with different dimension, in nTupla::includes method");

	for ( register unsigned i = 0; i < size; i++ )
		if ( pos.get(i) >= valTupla[i] || pos.get(i) < 0 )
			return false;

	return true;
}

bool nTupla::isInRegion( const nTupla &firstCell, const nTupla &lastCell) const
{

	MASSERTMSG( size == firstCell.dimension(), "Attemp to compare tuples with different dimension, in nTupla::isInRegion method");
	MASSERTMSG( size == lastCell.dimension(), "Attemp to compare tuples with different dimension, in nTupla::isInRegion method");

	for ( register unsigned i = 0; i < size; i++ )
		if ( valTupla[i] < firstCell.get(i) || valTupla[i] > lastCell.get(i) )
			return false;
	return true;
}

long nTupla::calculateIndex( const nTupla &dim, bool validateIncludes ) const
{
	//MASSERTMSG( dim.dimension() == size, "Attemp to calculate an Index for a tuple with different dimension size");
	if (dim.dimension() != size)
		return 0;


	if (validateIncludes)
		MASSERTMSG( dim.includes(*this), "Attemp to calculate an index of a tuple outside the dimension.");

	register int	previous = dim.valTupla[0];
	register long	index = valTupla[0], multip = 1;

	for (register unsigned i = 1; i < size; i++)
	{
		multip *= previous;
		index += valTupla[i] * multip;
		previous = dim.valTupla[i];
	}
	return index;
}

bool nTupla::contains( int elem ) const
{
	for ( register unsigned i = 0; i < size; i++ )
		if ( valTupla[i] == elem )
			return true;	// element found

	return false;			// element not found
}

void nTupla::minCoordToCoord( nTupla &t1, nTupla &t2 )
{
	MASSERTMSG( t1.dimension() == t2.dimension() && t1.dimension() == size, "Attemp to use tuples with different dimension, in nTupla::minCoordToCoord method");

	for ( register unsigned i = 0; i < size; i++)
		setElement(i, t2.get(i) < t1.get(i) ? t2.get(i) : t1.get(i));

	canonizada = false;
}

void nTupla::maxCoordToCoord( nTupla &t1, nTupla &t2 )
{
	MASSERTMSG( t1.dimension() == t2.dimension() && t1.dimension() == size, "Attemp to use tuples with different dimension, in nTupla::maxCoordToCoord method");

	for ( register unsigned i = 0; i < size; i++)
		setElement(i, t2.get(i) > t1.get(i) ? t2.get(i) : t1.get(i));

	canonizada = false;
}

nTupla &nTupla::operator = (const nTupla &nt)
{
	if (size > 0)
		delete [] valTupla;
	valTupla = new int[ size = nt.dimension() ];
	valTotalElem = 1;	

	for (register unsigned i = 0; i < size; i++)
	{
		valTupla[i] = nt.valTupla[i];
		valTotalElem *= valTupla[i];
	}
	canonizada = false;
	return *this;
}

/*******************************************************************
* Function Name: parseString
********************************************************************/
nTupla &nTupla::parseString( const string &cellStr )
{
	char    auxi[128], num[15], *curs = auxi;
	strcpy(auxi, cellStr.c_str());
	register int posi;

	while (*curs != '(' && *curs != 0)
		curs++;

	MASSERTMSG( *curs == '(', string("Invalid cell string format, '(' missing in " ) + cellStr ) ;
	curs++;

	while ( *curs != ')' && *curs != 0 )
	{
                // get the number
		posi = 0;
		while ( *curs != ')' && *curs != ',' && *curs != 0 )
		{
			num[posi++] = *curs;
			curs++;
		}
		num[posi++] = 0;
		this->add( str2Int( num ) );

                // Si hay una coma, la salteo
		if (*curs == ',')
			curs++;
	}
	MASSERTMSG( *curs == ')', string("Invalid cell string format, ')' missing in " ) + cellStr ) ;
	return *this ;
}

/*******************************************************************
* Function Name: set
* Description: Adds n values to the CellPosition
********************************************************************/
void nTupla::set( unsigned n, int value )
{
	int	*valAux = new int[size+n];	// Backup the tupla
	register unsigned	i = 0;

	for (; i < size; i++)
		valAux[i] = valTupla[i];

	size += n;
	for (; i < size; i++)
	{
		if (valAux[i] == 0)
			calcTotalElements();
		else
		{
			valTotalElem /= valAux[i];
			valTotalElem *= value;
		}

		valAux[i] = value;		// Adds the element
	}

	delete [] valTupla;
	valTupla = valAux;		// Reasigns the tupla

	canonizada = false;
}

/*******************************************************************
* Function Name: next
* Description: Calcula el siguiente (incremento en 1), considerando
*               la dimension para poder hacer los carry de la suma
*               Devuelve TRUE si se produjo un overflow, o false
*               si es posible hacer la suma
********************************************************************/
bool nTupla::next( const nTupla &dim )
{
	MASSERTMSG( dim.dimension() == size, "The dimension of the tuples must be equal in method NEXT");

	register int	cant, posUlt = size - 1;
	register bool	incremento = true;

	while ( posUlt >= 0 && incremento )
	{
		if ((cant = valTupla[posUlt] + 1) >= dim.get( posUlt ) )
			setElement( posUlt--, 0);
		else
		{
			setElement( posUlt, cant);
			incremento = false;
		}
	}

        // if posUlt < 0 then occurs an Overflow
	return posUlt < 0 ? true : false;
}

/*******************************************************************
* Function Name: fill
* Description: Sets the first n values
********************************************************************/
void nTupla::fill( unsigned n, int value )
{
	for (register unsigned i=0; i<n; i++)
		setElement(i, value);
}
