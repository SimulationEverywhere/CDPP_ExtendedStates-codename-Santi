/*******************************************************************
*
*  DESCRIPTION: class StandAloneMainSimulator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 30/8/2000
*
*******************************************************************/

#ifndef __STANDALONESIMULATOR_H
#define __STANDALONESIMULATOR_H

/** include files **/
#include <iostream>
#include "root.h"          // EventList 
#include "ini.h"           // class Ini
#include "loader.h"        // class SimLoader 
#include "ltranadm.h"      // class LocalTransAdmin
#include "parsimu.h"	   // class ParallelMainSimulator



/** declarations **/
class StandAloneMainSimulator : public ParallelMainSimulator
{
public:
	ParallelMainSimulator &run() ;
	
protected:
	friend class Coupled ;
	friend class Atomic ;
	
	// ** Methods ** //
	ParallelMainSimulator &loadModels( std::istream&, bool printParserInfo ) ;
	ParallelMainSimulator &loadExternalEvents( std::istream& ) ;
//	ParallelMainSimulator &log( std::ostream & ) ;
//	ParallelMainSimulator &output( std::ostream & ) ;
	ParallelMainSimulator &loadModel( Coupled &, Ini &, bool ) ;
	ParallelMainSimulator &loadPorts( Coupled &, Ini & ) ;
	ParallelMainSimulator &loadComponents( Coupled &, Ini &, bool ) ;
	ParallelMainSimulator &loadLinks( Coupled &, Ini & ) ;
	ParallelMainSimulator &loadCells( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadInitialCellValues( CoupledCell &, Ini & ) ;
	ParallelMainSimulator &loadInitialCellValuesFromFile( CoupledCell &parent, const std::string &fileName );
	ParallelMainSimulator &loadInitialCellValuesFromMapFile( CoupledCell &parent, const std::string &fileName );
	ParallelMainSimulator &loadDefaultTransitions( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadPortInTransitions( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadLocalZones( CoupledCell &, Ini &, bool ) ;
	
	ParallelMainSimulator &registerTransition(const LocalTransAdmin::Function &, Ini &, bool );
	ParallelMainSimulator &registerTransitionPortIn(const LocalTransAdmin::Function &, Ini &, bool, const std::string & );
	
	void showEvents( const EventList &events, std::ostream &out = std::cout ) ;
	
	
};	// class StandAloneMainSimulator

#endif   //__StandAloneMainSimulator_H
