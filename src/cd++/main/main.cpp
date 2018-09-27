/*******************************************************************
*
*    DESCRIPTION: main
*
*    DATE: 12/11/99
*    DATE: 30/08/2000 (v3)
*
*******************************************************************/

/** include files **/
#include <iostream>
#include <string>
#include "strutil.h"	// isNumeric
#include "stdaload.h"   // class StandAloneLoader
#include "parsimu.h"   // class ParallelMainSimulator
#include "pmodeladm.h"	// class SingleParallelModelAdm

using namespace std;

int main( int argc, char *argv[] )
{
	SimLoader *loader = NULL;

	try
	{


#ifndef __unix__
		loader = new StandAloneLoader(argc, argv);
#else
#ifdef __ONLY_STAND_ALONE_MODE__
		loader = new StandAloneLoader(argc, argv);
#else
		if( argc == 1 ) {
			cout << "PCD++" << endl;
			cout << "The network loader is not currently supported" << endl;
			cout << endl << "Please run pcd -h for help" << endl;

			return 0;
			//loader = new NetworkLoader(argc, argv);

		} else {
			loader = new StandAloneLoader(argc, argv) ;
		}
#endif
#endif

		ParallelMainSimulator::Instance().loader( loader );
		ParallelMainSimulator::Instance().run();

	} catch( const MException &e )
	{
		e.print(cerr);
		if (loader != NULL)
			loader->closeComm();
	} catch( ... )
	{
		cerr << "Unknown Exception!!! "  << endl; 
		if (loader != NULL)
			loader->closeComm();
	}

	if (loader != NULL) {
		delete loader;
	}


	return 0;
}

