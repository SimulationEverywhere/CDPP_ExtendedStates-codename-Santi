/*******************************************************************
*
*  DESCRIPTION: class ParallelMainSimulator
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel A. Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*         mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 12/6/1999 (v2)
*  DATE: 30/8/2000 (v3)
*
*******************************************************************/

#ifndef __SIMULATOR_H
#define __SIMULATOR_H

/** include files **/
#include <iostream>
#include "root.h"          // EventList 
#include "ini.h"           // class Ini
#include "loader.h"        // class SimLoader 
#include "ltranadm.h"      // class LocalTransAdmin

/** forward declarations **/
class Model ;
class Coupled ;
class Atomic ;
class CoupledCell ;

/** declarations **/
class ParallelMainSimulator
{
public:
	static ParallelMainSimulator &Instance() ;
	static ParallelMainSimulator &CreateParallel();
	static ParallelMainSimulator &CreateStandAlone();
	
	virtual ParallelMainSimulator &run()= 0;
	
	ParallelMainSimulator &loader( SimLoader *l )
	{sloader = l; return *this;}
	
	void registerNewAtomics() ;
	
	std::ostream &outputStream()
	{return loader()->outputStream();}
	
	std::ostream &logStream()
	{return loader()->logStream();}
	
	std::ostream &evalDebugStream()
	{return EvalDebug().Stream();}
	
	bool evalDebug()
	{return EvalDebug().Active();}
	
	bool existsParameter( const std::string &modelName, const std::string &parameterName ) const
	{return iniFile().exists( modelName, parameterName );}
	
	const std::string getParameter( const std::string &modelName, const std::string &parameterName ) const ;
	
	SimLoader *loader()
	{return sloader;}
	
	virtual ~ParallelMainSimulator(){};
	
protected:
	friend class Coupled ;
	friend class Atomic ;
	
	// ** class variables ** //
	static ParallelMainSimulator *instance ;
	
	// ** instance constants ** //
	const char separator ;
	const char *componentLabel ;
	const char *inPortsLabel ;
	const char *outPortsLabel ;
	const char *linksLabel ;
	
	// ** protected variables ** //
	SimLoader *sloader ;
	Ini ini;
	
	// ** Methods ** //
	
	ParallelMainSimulator() ;
	
	const Ini &iniFile() const
	{return ini;}
	
	bool splitString( const std::string &full, std::string &first, std::string &second, char separator );
	
};	// class ParallelMainSimulator


#endif   //__MainSimulator_H 
