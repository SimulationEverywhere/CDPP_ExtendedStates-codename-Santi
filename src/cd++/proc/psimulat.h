/*******************************************************************
*
*  DESCRIPTION: class ParallelSimulator
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 07/11/2000
*
*******************************************************************/

#ifndef __PSIMULATOR_H
#define __PSIMULATOR_H

/** include files **/
#include "pprocess.h"       	// class ParallelProcessor
#include "pSimulatorState.h"	// class ParallelSimulatorState

/** forward declarations **/
class Atomic ;

/** declarations **/
class ParallelSimulator: public ParallelProcessor
{
public:
	//Functions required by Warped
	void initialize();
	
	ParallelSimulator( Atomic * );	 // Default constructor
	
	//Functions that will not be redefined are commented out
	//ParallelProcessor &receive( const BasicExternalMessage * ) ;
	//ParallelProcessor &receive( const DoneMessage & );
	//ParallelProcessor &receive( const BasicOutputMessage *);
	
	ParallelProcessor &receive( const InitMessage     & ) ;
	ParallelProcessor &receive( const InternalMessage & ) ;
	ParallelProcessor &receive( const CollectMessage  & ) ;
	
	BasicState* allocateState();
	
	
};	// class ParallelSimulator

#endif   //__PSIMULATOR_H 
