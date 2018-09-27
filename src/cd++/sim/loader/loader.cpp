/*******************************************************************
*
*  DESCRIPTION: CLASS SimLoader
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 11/04/1999 (v2)
*  DATE: 29/08/2000 (v3)
*
*******************************************************************/

#include "loader.h"
#include "mpi.h"

using namespace std;

/*******************************************************************
* Function Name: openCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &SimLoader::openCommChanel(){
	
	//Init MPI.If not start up machine, this will update
	//the command line arguments
	physicalCommInit(&copyArgc , &copyArgv);
	
	machineID = physicalCommGetId();
	
	static_cast<StandAloneLoader*>(loader())->readCommandLine(argc, argv);
	
	return *instance;
}

/*******************************************************************
* Function Name: closeCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &SimLoader::closeCommChanel(){
	
	MPI_Finalize();
	return *instance;
}
