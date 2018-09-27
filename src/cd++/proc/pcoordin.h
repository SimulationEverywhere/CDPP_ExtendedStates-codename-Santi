/*******************************************************************
*
*  DESCRIPTION: class ParallelCoordinator, ParallelCoordinatorState
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 13/11/2000
*
*******************************************************************/

#ifndef __PCOORDINATOR_H
#define __PCOORDINATOR_H

/** include files **/
#include <set>
#include "pprocess.h"          // class Processor
#include "pCoordinatorState.h" // class ParallelCoordinatorState


/** forward declarations **/
class Coupled ;

/** declarations **/
class ParallelCoordinator: public ParallelProcessor 
{
public:

	//Functions that are required by Warped
	void initialize();

	BasicState* allocateState();

	//The following methods will not be redefined
	//ParallelProcessor &receive( const BasicExternalMessage * ) ;

	ParallelCoordinator( Coupled * ); //constructor
	ParallelProcessor &receive( const InitMessage & ) ;
	ParallelProcessor &receive( const InternalMessage & ) ;	
	ParallelProcessor &receive( const CollectMessage &  ) ;
	ParallelProcessor &receive( const BasicOutputMessage * ) ;
	ParallelProcessor &receive( const DoneMessage & ) ;


protected:

	ProcId parentId;

	typedef std::set<ProcId> ProcSet;
	ProcSet synchronizeList;


	ParallelCoordinator(const ParallelCoordinator & ) ;	// Copy constructor

	ParallelCoordinator& operator=(const ParallelCoordinator & )  ;	 // Assignment operator

	int operator==(const ParallelCoordinator & ) const ;	// Equality operator



	virtual bool rollbackCheck(const VTime& currentTime);

	virtual ParallelCoordinator &sortExternalMessage( const BasicExternalMessage& msg);

	virtual ParallelCoordinator &sortOutputMessage( const BasicOutputMessage& msg);

	virtual VTime calculateNextChange(const VTime& time) const;

	//Short cuts to processor state
	const int doneCount() const;

	ParallelCoordinator &doneCount( int );

	ParallelCoordinatorState::DependantList &dependants() const;

private:

	int donecount;


};	// class ParallelCoordinator

/*******************************************************************
* Function Name: Shortcuts to state variables
********************************************************************/
inline
const int ParallelCoordinator::doneCount() const{
	return donecount;
}

inline
ParallelCoordinator &ParallelCoordinator::doneCount( int d){
	donecount = d;
	return *this;
}

inline
ParallelCoordinatorState::DependantList& ParallelCoordinator::dependants() const {
	return *((ParallelCoordinatorState *)state->current)->dependants;
}



#endif   //__PCOORDINATOR_H 
