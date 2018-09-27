/*******************************************************************
*
*  DESCRIPTION: class ProcessorAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#include "strutil.h"          // lowerCase
#include "procadm.h"		  // base class
#include "sprocadm.h"		  // StandAloneProcessorAdmin
#include "pprocadm.h"		  // ParallelProcessorAdmin

using namespace std;

/** private data **/
ProcessorAdmin *SingleProcessorAdmin::instance( NULL ) ;

/** public functions **/
/*******************************************************************
* Function Name: CreateParallel
********************************************************************/

ProcessorAdmin &SingleProcessorAdmin::CreateParallel()
{
	if( !instance ) {
	
		instance = new ParallelProcessorAdmin();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: CreateStandAlone
********************************************************************/

ProcessorAdmin &SingleProcessorAdmin::CreateStandAlone()
{
	if( !instance ) {
		instance = new StandAloneProcessorAdmin();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: ~ProcessorAdmin()
* Description: destroys all the alive processors
********************************************************************/
ProcessorAdmin::~ProcessorAdmin()
{
}





