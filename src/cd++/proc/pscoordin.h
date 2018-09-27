/*******************************************************************
*
*  DESCRIPTION: class ParallelSlaveCoordinator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 24/01/2001
*
*******************************************************************/

#ifndef __PSCOORDINATOR_H
#define __PSCOORDINATOR_H

/** include files **/
#include <list>
#include "pcoordin.h"          // class Processor
#include "pCoordinatorState.h" // class ParallelCoordinatorState


/** forward declarations **/
class Coupled ;

/** declarations **/
class ParallelSCoordinator: public ParallelCoordinator
{
public:

	//Functions that are required by Warped
	void initialize();

	ParallelSCoordinator( Coupled * ); //constructor

	//The following commented methods will not be redefined.
	//BasicState* allocateState();
	//ParallelProcessor &receive( const InternalMessage & ) ;	
	//ParallelProcessor &receive( BasicExternalMessage * ) ;
	//ParallelProcessor &receive( BasicOutputMessage * ) ;

	ParallelProcessor &receive( const InitMessage & ) ;
	ParallelProcessor &receive( const DoneMessage & ) ;
	ParallelProcessor &receive( const CollectMessage & );





protected:

	ParallelSCoordinator(const ParallelSCoordinator & ) ;	// Copy constructor
	ParallelSCoordinator& operator=(const ParallelSCoordinator & )  ;	 // Assignment operator
	int operator==(const ParallelSCoordinator & ) const ;	// Equality operator

	ParallelCoordinator& sortOutputMessage  ( const BasicOutputMessage & );
	ParallelCoordinator& sortExternalMessage( const BasicExternalMessage & );
	virtual VTime calculateNextChange(const VTime& time) const;

};	// class Coordinator

#endif   //__PCOORDINATOR_H 
