/*******************************************************************
*
*  DESCRIPTION: class CellState
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez. 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 25/04/1999 (v2)
*
*******************************************************************/

#ifndef __CELLSTATE_H
#define __CELLSTATE_H

/** include files **/
#include "cellpos.h"       // class CellPosition
#include "real.h"

/** declarations **/
class CellState
{
public:
	CellState( nTupla &dim, bool wrapped = false ) ;	 //Default constructor

	virtual ~CellState();	// Destructor

	const Real &operator[]( CellPosition & ) const ;
	Real &operator[]( CellPosition & ) ;

	bool includes( const CellPosition &pos ) const
		{return isWrapped() || dimList->includes(pos);}

	bool isWrapped() const
			{return wrapped;}

	nTupla &dimension()
			{return *dimList;}

	void calcRealPos( CellPosition &pos );

	void print( std::ostream &, char ='?' ) const;
	void printFormatedList( std::ostream & ) const;
	
private:
	// ** instance variables ** //
	Real **matrix ;
	nTupla *dimList ;
	bool wrapped ;

	CellState( const CellState & ) ;	// copy constructor no definido exprofeso

};	// class CellState

/** inline **/
inline
void CellState::calcRealPos( CellPosition &pos )
{
	if ( this->isWrapped() )
		pos.canonizar( *dimList );
}

#endif   //__CELLSTATE_H 
