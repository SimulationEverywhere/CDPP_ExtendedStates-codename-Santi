/*******************************************************************
*
*  DESCRIPTION: class ParallelMainSimulator
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
*  DATE: 12/06/1999 (v2)
*  DATE: 30/08/2000 (v3)
*
*******************************************************************/

/** include files **/
#include <fstream>
#include <strstream>
#include "parsimu.h"   // base header
#include "stdasimu.h"	// StandAloneMainSimulator
#include "parsimu.h"	// ParallelMainSimulator
#include "ini.h"        // class Ini
#include "pmodeladm.h"   // class SingleParallelModelAdm
#include "strutil.h"    // lowerCase, str2Int
#include "coupled.h"    // class Coupled
#include "zone.h"       // class zone
#include "flatcoup.h"   // class FlatCoupledCell
#include "tdcell.h"	//
#include "idcell.h"	//

using namespace std;

ParallelMainSimulator* ParallelMainSimulator::instance(NULL);

/*******************************************************************
* Function Name: Constructor
********************************************************************/

ParallelMainSimulator::ParallelMainSimulator()
: separator( '@' )
, componentLabel( "components" )
, inPortsLabel("in")
, outPortsLabel("out")
, linksLabel("link")
, sloader( NULL )
{
	registerNewAtomics();
}


/*******************************************************************
* Function Name: CreateParallel
********************************************************************/

ParallelMainSimulator &ParallelMainSimulator::CreateParallel()
{
	if( !instance ) {
	
		instance = new ParallelMainSimulator();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: CreateStandAlone
********************************************************************/

ParallelMainSimulator &ParallelMainSimulator::CreateStandAlone()
{
	if( !instance ) {
		instance = new StandAloneMainSimulator();        	
	}
	return *instance;
}

/*******************************************************************
* Function Name: getParameter
********************************************************************/
const string ParallelMainSimulator::getParameter( const string &modelName, const string &parameterName ) const
{
	const Ini::IdList &idList( iniFile().definition( modelName, parameterName ) );

	MASSERTMSG( !idList.empty(), "Parameter " + parameterName + " without value" );

	return idList.front();
}

/*******************************************************************
* Function Name: Instance
********************************************************************/

ParallelMainSimulator &ParallelMainSimulator::Instance()
{
	MASSERTMSG( instance, "No simulator instance (Parallel or StandAlone) has been defined." );
	return *instance;
}

/*******************************************************************
* Method: splitString
********************************************************************/
bool ParallelMainSimulator::splitString( const string &full, string &first, string &second, char separator )
{
	first = second = "";
	register int index = full.find( separator );
	if( index >= 0 )
	{
		first = full.substr( 0, index );
		second = full.substr( index + 1 );
	} else
		first	= full;

	return index >= 0;
}
