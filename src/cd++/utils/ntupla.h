/*******************************************************************
*
*  DESCRIPTION: class nTupla
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 24/04/1999 (v2)
*
*******************************************************************/

#ifndef __NTUPLA_H
#define __NTUPLA_H

/** include files **/
#include <list>
#include "except.h"

/** forward declarations **/
#define	DIM_WIDTH	1
#define	DIM_HEIGHT	0

/** declarations **/
class nTupla
{
public:
	~nTupla()			// Destructor
	{ delete [] valTupla; }

	nTupla()			// Constructor
		:valTotalElem( 1 )
		,size(0)
		,canonizada(true)
	{ valTupla = new int[1]; }

	nTupla(const nTupla &pos);	// Constructor
	nTupla(const std::string &str);	// Constructor
	nTupla(unsigned dim, int value);
					// Constructor. Crea una nTupla conteniendo dim elementos con el valor indicado

	bool	operator == ( const nTupla & ) const;
	bool	operator <  ( const nTupla & ) const;
	bool   operator <= ( const nTupla & ) const;

	nTupla  &operator = (const nTupla & );

	nTupla  &operator = (const std::string &cellStr)
	{ parseString(cellStr); return *this; }

	nTupla  *operator += ( const nTupla & );
	nTupla  *operator -= ( const nTupla & );

	unsigned dimension() const	// Returns the dimension of the tupla
	{ return size; }

	int get(unsigned n) const;	// Returns the nth element from the tupla
					// n must be between 0 and k-1, where
					// k is the dimension
					// NOTE: n = 0 = width
					//	 n = 1 = height

	void	setElement(unsigned pos, int n);
					// Sets the pos-th element of
					// the tuple with the n value

	void	set(unsigned n, int value);
					// Adds n elements with
					// value to the tupla

	void	fill(unsigned n, int value);
					// Sets the first n elements
					// with the specified values

	void	add(int n);	// Adds an element to the tupla,
				// in the last position

	void	add(int n1, int n2);
				// Adds the elements n1 and n2 to the tupla,
				// in the last position
				// (the last element is n2)

	nTupla *addFirst(int n);	// Adds an element to the tupla,
					// in the first position

	nTupla *addFirst(int n1, int n2);
					// Adds the elements n1 and n2 to the
					// tupla, in the first position
					// (first n1, second n2, and the old tupla)

	nTupla	&canonizar( const nTupla &dim );	// Aplica modulos a todos los
							// elementos de la tupla

	long	totalElements() const	// Returns the total number of cells respected to the tupl
	{ return valTotalElem; }

	bool	includes( const nTupla &pos ) const;
					// Returns TRUE if this tupla
					// contains the pos tupla.

	long	calculateIndex( const nTupla &dim, bool validateIncludes = true ) const;
					// Calcula el indice, para
					// una matriz capaz de almacenar
					// la dimension especificada

	bool	contains( int elem ) const;	// Returns true if the tuple
						// contains the element elem.

	std::string print() const;

	void minCoordToCoord( nTupla &t1, nTupla &t2 );
						// Calcula el minimo coordenada
						// a coordenada entre las tuplas
						// t1 y t2. En this deja el
						// resultado
	void maxCoordToCoord( nTupla &t1, nTupla &t2 );
						// Analogo a minCoordToCoord

	bool next( const nTupla &dim );		// Avanza a la siguiente
						// tupla (incremento en 1
						// con overflow segun la
						// dimension)

	bool isInRegion( const nTupla &firstCell, const nTupla &lastCell) const;
						//Returns True if the cell is in the n-plane region	
						//defined by firstCell and lastCell
private:
	void	calcTotalElements();		// Recalcula el valTotalElem

	nTupla	&parseString( const std::string &str );

	int *tupla()
	{ return valTupla; }

	bool isCanonizada() const
	{ return canonizada; }

	// Data Type
	int		*valTupla;
	long		valTotalElem;
	unsigned	size;
	bool		canonizada;

}; // nTupla


// Inline functions
inline
	std::ostream &operator << (std::ostream &os, const nTupla &t)
{
	os << t.print();
	return os;
}

#endif	// nTupla
