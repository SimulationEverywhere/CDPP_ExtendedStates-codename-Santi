/*******************************************************************
*
*  DESCRIPTION: class ParallelMainSimulator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 9/5/2002
*
*******************************************************************/

#ifndef __PARALLELSIMULATOR_H
#define __PARALLELSIMULATOR_H

/** include files **/
#include <iostream>
#include "proot.h"          	// EventList
#include "ini.h"           		// class Ini
#include "pini.h"	   		// class PIni
#include "loader.h"        		// class SimLoader 
#include "ltranadm.h"      		// class LocalTransAdmin
#include "log.h"
#include "LogicalProcess.h"	// Warped logical process



/** declarations **/
class ParallelMainSimulator 
{
public:
	static ParallelMainSimulator &Instance() ;

	ParallelMainSimulator &loader( SimLoader *l )
	{sloader = l; return *this;}

	void registerNewAtomics() ;

	std::string outputName()
	{return loader()->outputName();}

	std::string logName()
	{return Log::Default.filename();}

	std::ostream &debugStream();

	std::ostream &evalDebugStream()
	{return EvalDebug().Stream();}

	bool evalDebug()
	{return EvalDebug().Active();}

	bool existsParameter( const std::string &modelName, const std::string &parameterName ) const
	{return iniFile().exists( modelName, parameterName );}

	const std::string getParameter( const std::string &modelName, const std::string &parameterName ) const ;

	SimLoader *loader()
	{return sloader;}

	ParallelMainSimulator &run();

	int getMachineID();

	virtual ~ParallelMainSimulator();

protected:
	friend class ParallelRoot;
	friend class CoupledCell;
	friend class Coupled ;
	friend class Atomic ;
	friend class Model;

	// ** class variables ** //
	static ParallelMainSimulator *instance ;

	// ** instance constants ** //
	const char separator ;
	const char *componentLabel ;
	const char *inPortsLabel ;
	const char *outPortsLabel ;
	const char *linksLabel ;

	// ** Methods ** //

	ParallelMainSimulator() ;

	// ** Methods ** //

	ParallelMainSimulator &loadModels( std::istream&, std::istream&, bool printParserInfo ) ;
	ParallelMainSimulator &setUpLogicalProcess();
	ParallelMainSimulator &loadExternalEvents( std::istream& ) ;
	ParallelMainSimulator &log( std::ostream & ) ;
	ParallelMainSimulator &output( std::ostream & ) ;
	ParallelMainSimulator &loadModel( Coupled &, bool ) ;
	ParallelMainSimulator &loadPorts( Coupled & ) ;
	ParallelMainSimulator &loadComponents( Coupled &, bool ) ;
	ParallelMainSimulator &loadLinks( Coupled & ) ;
	ParallelMainSimulator &loadCells( CoupledCell &, bool ) ;
	ParallelMainSimulator &loadStateVariables( CoupledCell &, bool ) ;
	ParallelMainSimulator &loadMachines( Atomic & );
	ParallelMainSimulator &loadMachines (Coupled & );
	ParallelMainSimulator &loadMachines( CoupledCell &coupledcell );
	ParallelMainSimulator &loadInitialCellValues( CoupledCell & ) ;
	ParallelMainSimulator &loadInitialCellValuesFromFile( CoupledCell &parent, const std::string &fileName );
	ParallelMainSimulator &loadInitialCellValuesFromMapFile( CoupledCell &parent, const std::string &fileName );
	ParallelMainSimulator &loadStateVariables( CoupledCell & ) ;
	ParallelMainSimulator &loadInitialVariablesValues( CoupledCell & ) ;
	ParallelMainSimulator &loadInitialVariablesValuesFromFile( CoupledCell &parent, const std::string &fileName );
	ParallelMainSimulator &loadDefaultTransitions( CoupledCell &, bool ) ;
	ParallelMainSimulator &loadPortInTransitions( CoupledCell &, bool ) ;
	ParallelMainSimulator &loadLocalZones( CoupledCell &, bool ) ;
	ParallelMainSimulator &registerTransition(const LocalTransAdmin::Function &, bool );
	ParallelMainSimulator &registerTransitionPortIn(const LocalTransAdmin::Function &, bool, const std::string & );
	ParallelMainSimulator &afterInitialize();

#ifndef KERNEL_NOTIME
	ParallelMainSimulator &calculateGVT()
	{
		lp->calculateGVT();
		return *this;
	}
#endif

	void showEvents( const EventList &events, std::ostream &out = std::cout ) ;
	void showModelPartition( std::ostream &out = std::cout );
	void showLogInfo( std::ostream &out = std::cout );

	bool splitString( const std::string &full, std::string &first, std::string &second, char separator );

	const Ini &iniFile() const
	{return ini;}

	const PIni &pIniFile() const
	{ return pini; }


private:

	// ** instance variables ** //
	SimLoader *sloader ;	
	std::ostream *vDebugStream;

	Ini ini;
	PIni pini;

	LogicalProcess *lp;


};	// class ParallelMainSimulator

/*inline*/
inline
int ParallelMainSimulator::getMachineID() {
	return loader()->machineID;
}


#endif   //__ParallelMainSimulator_H
