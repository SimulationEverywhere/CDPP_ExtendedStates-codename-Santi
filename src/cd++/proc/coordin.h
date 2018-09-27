/*******************************************************************
*
*  DESCRIPTION: class Coordinator
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 04/6/1999 (v2)
*
*******************************************************************/

#ifndef __COORDINATOR_H
#define __COORDINATOR_H

/** include files **/
#include "process.h"          // class Processor

/** forward declarations **/
class Coupled ;

/** declarations **/
class Coordinator: public Processor 
{
public:
	Coordinator( Coupled * ); //constructor

	Processor &receive( const InitMessage & ) ;

	Processor &receive( const InternalMessage & ) ;

	Processor &receive( const OutputMessage & ) ;

	Processor &receive( const ExternalMessage & ) ;

	Processor &receive( const DoneMessage & ) ;

protected:
	Coordinator(const Coordinator & ) ;	// Copy constructor

	Coordinator& operator=(const Coordinator & )  ;	 // Assignment operator

	int operator==(const Coordinator & ) const ;	// Equality operator

	Processor &id( const ProcId &pid ) ;

	const ProcId &inminentChild() const
			{return inminent;}

	Coordinator &inminentChild( const ProcId & ) ;

	long doneCount;
	long receivedX;

private:
	ProcId inminent ;

	Coordinator &recalcInminentChild() ;

};	// class Coordinator


/** inline **/
inline
Coordinator &Coordinator::inminentChild( const ProcId &mid ) 
{
	inminent = mid ;
	return *this ;
}

#endif   //__COORDINATOR_H 
