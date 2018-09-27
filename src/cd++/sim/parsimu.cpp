/*******************************************************************
*
*  DESCRIPTION: class ParallelMainSimulator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 30/8/2000
*
*******************************************************************/

/** include files **/
#include <fstream>
#include <strstream>
#include <map>
#include <cstring>
#include "parsimu.h"		// base header
#include "stdaload.h"
#include "ini.h"       	// class Ini
#include "pmodeladm.h"	// class ParallelModelAdm
#include "strutil.h"    	// lowerCase, str2Int
#include "coupled.h"    	// class Coupled
#include "zone.h"       	// class zone
#include "cellpartition.h"  // class CellPartition
#include "flatcoup.h"   	// class FlatCoupledCell
#include "tdcell.h"		//
#include "idcell.h"		//
#include "statevars.h"

#ifdef MPI
#include "mpi.h"		// MPI_Routines
#endif

using namespace std;

//define SIMUNTIL. Set the lp simulation stop time to Infinity. 
//The actual simulation stop time will be controlled by the Parallel Root
//processor.
const VTime LogicalProcess::SIMUNTIL = PINFINITY;

//Allocate static variables
ParallelMainSimulator *ParallelMainSimulator::instance(NULL);

//getLineOnFile
int getLineOnFile(FILE *fileIn, char *linea);

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
	, vDebugStream ( NULL )
{
	registerNewAtomics();
}
/*******************************************************************
* Function Name: Destructor
********************************************************************/

ParallelMainSimulator::~ParallelMainSimulator()
{
	//Release acquired memory
	if ( vDebugStream ) {
		(*vDebugStream) << flush;
		delete vDebugStream; 
	}
}
/*******************************************************************
* Function Name: Instance
********************************************************************/

ParallelMainSimulator &ParallelMainSimulator::Instance()
{
	if( !instance ) {

		instance = new ParallelMainSimulator();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: run
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::run()
{


	if( !loader() )
	{
		MException e( "The ParallelMainSimulator loader not found!" ) ;
		e.addText( "The loader must be seted before running the simulation." ) ;
		MTHROW( e ) ;
	}

	//Open the comm chanel to get the machine ID and
	//the command line parameters (if machine is not start up machine).
	loader()->openComm();

	//Read the command line arguments.

	loader()->loadData();

	ostream& out = debugStream();

	//Load all the models and the model partition.
	loadModels( loader()->modelsStream(), loader()->partitionStream(), loader()->printParserInfo() );

	//Display the model partition
	showModelPartition(out);

	//If a log file is to be created and this is the starting machine, save log file info
	//for drawlog
	if ( getMachineID() == 0 && logName() != "/dev/null" )
	{
		fstream logInfo ( logName().c_str() , ios::out );
		showLogInfo(logInfo);
	}

	//Set up de Warped logical process. This will create all the local processors.	
	setUpLogicalProcess();

	SingleParallelProcessorAdmin::Instance().showProcessors(out);


	//The root processor will run on machine with id 0 (starting machine)
	//If this is machine with Id 0, then setup the Root processor


	out << "All objects have been registered!" << endl << flush;


	//Tell the LP that all processors have been registered
	//This will also initialize all the processor current state!!!
	lp->allRegistered();	

	//Call afterInitialize for all local Models.
	out << "After Initialize...." << flush;

	afterInitialize();

	out << "OK" << endl << flush;

	if (getMachineID() == 0 ) {

		out << "Loading external events..." << endl;

		loadExternalEvents( loader()->eventsStream() );

		out << "Setting stop time..." << endl;

		ParallelRoot::Instance().stopTime( loader()->stopTime() );

		ParallelRoot::Instance().rootSimulate();
	}

	//Start the simulation
	lp->simulate();

	// Results (do not confuse with log file) are only written to
	//the starting machine

	if (getMachineID() == 0 ) 
		loader()->writeResults();

	delete lp;	

	//When the lp is deleted, the Comm Chanel is closed

	return *this;
}

/*******************************************************************
* Function Name: loadModels
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadModels( istream &file, istream &pfile, bool printParserInfo )
{

	ini.parse( file );		
	pini.parse( pfile );	//Model partition file

	//Create Root Model and register its processor Id
	//The root will run in machine 0
	Model &root(SingleParallelModelAdm::Instance().newRoot());
	root.addMachine(0);
	root.setMachineProcId(0, ParallelProcessorAdmin::rootId);


	//Create top model.
	Coupled &top(SingleParallelModelAdm::Instance().newCoupled( "top" ));

	loadModel( top, printParserInfo );

	if ( top.isLocalMaster() ) 
		top.parentId(SingleParallelProcessorAdmin::Instance().rootId);
	else
		top.parentId( top.masterId());


	return *this;
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
* Function Name: loadModel
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadModel( Coupled &parent, bool printParserInfo )
{

	// cargamos los ports
	loadPorts( parent);


	// los componentes
	if( parent.type() == Coupled::cell )
		loadCells( static_cast<CoupledCell&>( parent ), printParserInfo ) ;
	else
		loadComponents( parent, printParserInfo ) ;

	// los links
	loadLinks( parent) ;

	// At this time, the links are created. Now, we load the portInTransitions
	if ( parent.type() == Coupled::cell) 
	{
		loadPortInTransitions( static_cast<CoupledCell&>(parent), printParserInfo ) ;
		loadMachines( static_cast<CoupledCell&>( parent )) ;
	}
	else
	{
		loadMachines ( parent );
	}

	return *this;
}


/*******************************************************************
* Function Name: loadPorts
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadPorts( Coupled &parent )
{
	string modelName( lowerCase( parent.description() ) ) ;

	Ini::IdList::const_iterator cursor ; 

	// ports in
	if ( ini.exists( modelName, ParallelMainSimulator::inPortsLabel ) )
	{
		const Ini::IdList &ilist( ini.definition( modelName, ParallelMainSimulator::inPortsLabel ) ) ;
		for( cursor = ilist.begin() ; cursor != ilist.end() ; cursor ++ )
			parent.addInputPort( *cursor ) ;
	}

	// ports out
	if( ini.exists( modelName, ParallelMainSimulator::outPortsLabel ) )
	{
		const Ini::IdList &ilist( ini.definition( modelName, ParallelMainSimulator::outPortsLabel ) ) ;
		for( cursor = ilist.begin() ; cursor != ilist.end() ; cursor ++ )
			parent.addOutputPort( *cursor ) ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadComponent
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadComponents( Coupled &parent, bool printParserInfo )
{

	string modelName( lowerCase( parent.description() ) ) ;

	bool exist = ini.exists( modelName, ParallelMainSimulator::componentLabel ) ;

	if( !exist )
	{
		MException e( string("Ini: Coupled Model without components!, Label: ") + modelName + " / " + ParallelMainSimulator::componentLabel ) ;
		MTHROW( e ) ;
	} else
	{
		const Ini::IdList &ilist( ini.definition( modelName, ParallelMainSimulator::componentLabel ) ) ;

		Ini::IdList::const_iterator cursor ;

		for( cursor = ilist.begin() ; cursor != ilist.end() ; cursor ++ )
		{
			string mName, atomicType ;

			bool isAtomic = splitString( *cursor, mName, atomicType, ParallelMainSimulator::separator ) ;

			if( isAtomic )
			{
				Atomic &model( SingleParallelModelAdm::Instance().newAtomic( atomicType, mName ) ) ;
				parent.addModel( model ) ;
				loadMachines ( model );
			} else
			{
				// es un acoplado
				Coupled *coupled ;

				// Si es un acoplado puede ser un CoupledCell
				if( ini.exists( mName, "type" ) )
				{
					string type( ini.definition( mName, "type" ).front() ) ;

					if( type == "cell" )
						coupled = &( SingleParallelModelAdm::Instance().newCoupledCell( mName ) ) ;
					else if( type == "flat" ) {
						MASSERTMSG( false, "This version of CD++ does not support flat cell models." ) ;
						coupled = &( SingleParallelModelAdm::Instance().newFlatCoupledCell( mName ) ) ;
					}
					else
						MASSERTMSG( false, "Invalid coupled type " + type + "!" ) ;
				} else
					coupled = &( SingleParallelModelAdm::Instance().newCoupled( mName ) ) ;

				parent.addModel( *coupled ) ;

				loadModel( *coupled, printParserInfo ) ;

			}
		}
	}
	return *this ;
}
/*******************************************************************
* Function Name: loadMachines
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadMachines( Atomic &atomic )
{

#ifdef MPI

	int machineCount;
	PIni::MachineList::const_iterator machinesCursor;
	PIni::IdList::const_iterator modelsCursor;

	machineCount = 0;
	for (machinesCursor = (pini.machines()).begin(); machinesCursor != (pini.machines()).end()
	; machinesCursor++ )
	{

             	//Look for a the atomic in this machine 	
		for(modelsCursor = (machinesCursor->second).begin();
		modelsCursor != (machinesCursor->second).end();
		modelsCursor++)
		{
			if ( *modelsCursor == atomic.description()) {
				machineCount++;

				MASSERTMSG( machineCount == 1, "Ambiguos machine location for atomic: " +
					atomic.description());

				atomic.addMachine( (MachineId) machinesCursor->first);

				//Set processor Id for the atomic simulator.

				atomic.setMachineProcId( machinesCursor->first, 
					static_cast< ParallelProcessorAdmin& > (
						SingleParallelProcessorAdmin::Instance()).newId());

			}//if

		}//for modelsCursor

	} //for machinesCursor

	//The atomic must be running in one machine
	MASSERTMSG( machineCount == 1, "No machine has been specified to run atomic: " +
		atomic.description());
#else

	atomic.addMachine( 0 );

	atomic.setMachineProcId( 0 , 
		static_cast< ParallelProcessorAdmin& > (
			SingleParallelProcessorAdmin::Instance()).newId());

#endif
	return *this;

} //loadMachines

/*******************************************************************
* Function Name: loadMachines
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadMachines( Coupled &coupled )
{

	Coupled::PModelList::const_iterator childrenCursor;
	Model *model;


	for ( childrenCursor = coupled.childModels().begin();
	childrenCursor != coupled.childModels().end(); childrenCursor++) {

		model = *childrenCursor;

		MachineId mid = model->machineForProcId( model->masterId() );

		coupled.addMachine( mid );

		//If no processorId has been set, set processor Id.
		if ( coupled.procInMachine( mid ) 
				== ParallelProcessor::InvalidId ){

			coupled.setMachineProcId( mid, 
				SingleParallelProcessorAdmin::Instance().newId());

		} //if 

	}

	//Now, for each child model, set up the parent/child relationship

	for ( childrenCursor = coupled.childModels().begin();
	childrenCursor != coupled.childModels().end(); childrenCursor++) {

		// parentId will be set for parallel simulation to the following:
		// For atomic models: if it is local, parentId = coupled.localProc
		// if it is not local, it will be the processor of coupled running
		// on the machine where the atomic is running.
		// For coupled child models. The same as atomic, but taking only into
		// account the location of the master processor.

		model = *childrenCursor;

		if ( model->isLocalMaster() )
			model->parentId( coupled.localProc() );
		else
			model->parentId( coupled.procInMachine (model->machineForProcId( model->masterId() ) ) );

	}	

	return *this;

} //loadMachines

/*******************************************************************
* Function Name: loadMachines
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadMachines( CoupledCell &coupledcell )
{

#ifdef MPI

	//The following must be done:
	//1. Collect the CellPartitions for all the machines the model will be
	//	running on.As each partition is created, add a machine to the coupled cell
	//	and generate a new procId.
	//2. For each cell, set the machine where it should run. Verify no
	//	cell will run on two machines and that there is at least one
	//	machine for every cell. 
	//	Some cells will not have an AtomicCell instance on the local machine.
	//	For such cells, do not set a local machine. Increment the ProcId also.
	//     This is important to
	//	maintain the consistency of ProcId's on each machine.

	//1. Let's start collecting the partition information
	typedef map< MachineId, CellPartition *, less< MachineId > > CellsMap;
	CellsMap cellsMap;

	for(PIni::MachineList::const_iterator machines = pini.machines().begin();
	machines != pini.machines().end(); machines++ )
	{
		CellPartition *partition = new CellPartition(coupledcell.dimension(), coupledcell.borderWrapped());

		//Iterate along the list of models defined for the current machine
		for (PIni::IdList::const_iterator cursor = machines->second.begin();
		cursor != machines->second.end();
		cursor++) {

			string modelName, zone;

			int index = (*cursor).find( '(' );
			if( index >= 0 )
			{
				modelName= (*cursor).substr( 0, index );
				zone = (*cursor).substr( index );

				if ( modelName == coupledcell.description() )
					partition->addLocalZone( zone );

			}

		} //for

		//Add the cell partition to the cellsMap
		if( partition->totalElements() > 0 ) {

			cellsMap[machines->first] = partition;

			//Create a new ProcId
			//This has to be done before calling coupledCell->setCellMachine

			coupledcell.setMachineProcId(machines->first, 
				SingleParallelProcessorAdmin::Instance().newId());

		} else {
			delete partition;	
		}

	}

	//2. For each cell, set the machine where it should run

	CellPosition 	counter( coupledcell.dimension().dimension(),0);
	bool overflow = false;

	while ( !overflow ){

		int machineCount = 0;

		//Get the procId for the cell
		ProcId procId = SingleParallelProcessorAdmin::Instance().newId();

		for( CellsMap::const_iterator cellmap = cellsMap.begin();
		cellmap != cellsMap.end();
		cellmap++ ) 
		{
			if( cellmap->second->localCellsInclude( counter )) {

				machineCount++;
				MASSERTMSG( machineCount == 1, "Ambiguos machine location for cell: " +
					counter.print());

				//SetCellMachine will also create the parent - child 
				//relationship.

				coupledcell.setCellMachine( counter, cellmap->first , procId);
			}

		}

		MASSERTMSG( machineCount == 1, "No machine has been speciefied for cell: " +
			counter.print());

		overflow = counter.next( coupledcell.dimension() );
	}//while

	//Now, delele all the cellpartitions that were just created
	for( CellsMap::const_iterator cellmap = cellsMap.begin();
	cellmap != cellsMap.end();
	cellmap++ ) 
		delete cellmap->second;

#else

	CellPosition 	counter( coupledcell.dimension().dimension(),0);
	bool overflow = false;

	//Set the machine for the coupled cell model
	coupledcell.setMachineProcId( 0, SingleParallelProcessorAdmin::Instance().newId());

	while ( !overflow ){

		//Get the procId for the cell
		ProcId procId = SingleParallelProcessorAdmin::Instance().newId();

		coupledcell.setCellMachine( counter, 0 , procId);

		overflow = counter.next( coupledcell.dimension() );
	}//while



#endif

	return *this;

} //loadMachines


/*******************************************************************
* Function Name: loadCells
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadCells( CoupledCell &parent, bool printParserInfo )
{
	string modelName( lowerCase( parent.description() ) ) ;

	// get the width and height
	if ( ini.exists( modelName,  "dim" ) )
	{
		MASSERTMSG( !ini.exists( modelName, "width" ) && !ini.exists( modelName, "height" ), "The DIM clause can't coexist with the WITDH or the HEIGHT clauses");
		Ini::IdList	dimension = ini.definition( modelName, "dim" );
		CellPosition	cp( ini.join(dimension) );
		nTupla		dims( cp );

		MASSERTMSG( dims.dimension() >= 2, "The tuple defined with the DIM clause is invalid. It must have 2 or more elements.");
		parent.dim( dims );
	}
	else
	{
		MASSERTMSG( !ini.exists( modelName, "dim" ), "The DIM clause can't coexist with the WITDH or the HEIGHT clauses");
		int width  = str2Int( ini.definition( modelName, "width"  ).front() ) ;
		int height = str2Int( ini.definition( modelName, "height" ).front() ) ;

		parent.dim( DIM_WIDTH, width );
		parent.dim( DIM_HEIGHT, height );
	}

	loadStateVariables( parent ) ;

	const std::string &initial_val_str = ini.definition( modelName, "initialvalue" ).front();
	value_ptr initial_val = AbstractValue::from_string(initial_val_str);
	parent.initialCellValue(*initial_val);

	bool inertial = ini.definition( modelName, "delay" ).front() == "inertial" ;
	parent.inertialDelay( inertial ) ;

	bool wrapped = ini.definition( modelName, "border" ).front() == "wrapped" ;
	parent.borderWrapped( wrapped ) ;

	int milSec( str2Int( ini.definition( modelName, "defaultDelayTime" ).front() ) ) ;
	parent.defaultDelay( VTime( 0, 0, 0, milSec ) ) ;

	CellPositionList neighborsList ;

	const Ini::IdList &neighbors( ini.definition( modelName, "neighbors" ) ) ;

	for( Ini::IdList::const_iterator cursor = neighbors.begin() ; cursor != neighbors.end() ; cursor++ )
		neighborsList.push_back( CellPosition( *cursor ) ) ; 

	loadDefaultTransitions( parent, printParserInfo ) ;

	//Now, before creating the cells, create the local partition
	CellPartition *partition = new CellPartition (parent.dimension(), parent.borderWrapped());

#ifdef MPI
	//Iterate along the list of models defined for the current machine
	for (PIni::IdList::const_iterator cursor = pini.machine(getMachineID()).begin();
	cursor != pini.machine(getMachineID()).end();
	cursor++) {

		string modelName, zone;

		int index = (*cursor).find( '(' );
		if( index >= 0 )
		{
			modelName= (*cursor).substr( 0, index );
			zone = (*cursor).substr( index );

			if ( modelName == parent.description() )
				partition->addLocalZone( zone );

		}

	} //for

#else

	//Create a string zone that includes all the cells of the model

	CellPosition zero( parent.dimension().dimension(),0);
	CellPosition last = parent.dimension();

	for (unsigned i = 0; i < last.dimension(); i++)
		last.setElement(i, last.get(i)-1);

	string zone;
	zone = zero.print() + ".." + last.print();

	partition->addLocalZone( zone );

#endif

	/* Read the NeighborChange ports */
	list<string> NCPorts;
	
	if ( ini.exists( modelName, "neighborports" ) ) {
		Ini::IdList::const_iterator cursor;
		const Ini::IdList &nc_ports( ini.definition( modelName, "neighborports" ) ) ;

		for(cursor = nc_ports.begin(); cursor != nc_ports.end(); cursor++ )
			NCPorts.push_back( *cursor ) ; 
	}


	//Create the cells. The CellPartition will be owned by the CoupledCell model
	//so do not delete it.
	parent.createCells( neighborsList, partition, NCPorts );

	loadLocalZones( parent,printParserInfo ) ;

	//The initial Cell Values will be loaded after the processor has
	//been initialized.
	//loadInitialCellValues( parent ) ;

	return *this ;
}

/*******************************************************************
* Function Name: loadLinks
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadLinks( Coupled &parent )
{
	string modelName( lowerCase( parent.description() ) ) ;

	Ini::IdList::const_iterator cursor ; 

	// links ( port(@model)? )
	if( ini.exists( modelName, ParallelMainSimulator::linksLabel ) )
	{
		const Ini::IdList &ilist( ini.definition( modelName, ParallelMainSimulator::linksLabel ) ) ;
		for( cursor = ilist.begin() ; cursor != ilist.end() ; cursor ++ )
		{
			string sourceName, sourcePort, destName, destPort;

			if( !splitString( *cursor, sourcePort, sourceName, ParallelMainSimulator::separator ) )
				sourceName = modelName ;

			cursor++;

			// the cursor must be valid
			if( !splitString( *cursor, destPort, destName, ParallelMainSimulator::separator ) )
				destName = modelName ;

			// addInfluence is a virtual function!
			// Every class: Coupled, CoupledCell and FlatCoupledCell
			// will know how to handle it's own case

			parent.addInfluence( sourceName, sourcePort, destName, destPort ) ;
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadExternalEvents
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadExternalEvents( istream  &fileIn )
{

	istream_iterator<string> cursor( fileIn ) ;

	try
	{
		while( cursor != istream_iterator<string>() )
		{
			VTime time( *cursor ) ;
			cursor ++ ;

			Port &port( SingleParallelModelAdm::Instance().model("top").port( *cursor ) ) ;
			cursor ++ ;

			ParallelRoot::Instance().addExternalEvent( time, port, *AbstractValue::from_string( *cursor ) ) ;
			cursor++ ;
		} 
	} catch( InvalidPortRequest &e )
	{
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		throw e ;
	}
	this->showEvents( ParallelRoot::Instance().events() ) ;

	return *this;
}

/*******************************************************************
* Function Name: loadInitialCellValues
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadInitialCellValues( CoupledCell &parent )
{
	if( ini.exists( parent.description(), "initialRow" ) )
	{
		MASSERTMSG( parent.dimension().dimension() == 2, "The initialRow clause must only be used when the dimension is 2");

		const Ini::IdList &rowsList( ini.definition( parent.description(), "initialRow" ) ) ;

		register unsigned row = 0;
		Ini::IdList::const_iterator cursor = rowsList.begin();

		while ( cursor != rowsList.end() )
		{
			// the first value is the row number
			row = str2Int( (*cursor) ) ;
			MASSERTMSG( row < parent.dim(DIM_HEIGHT), "The number of row for initialRow is out of range. It's " + int2Str(row) + " and must be in [ 0, " + int2Str( parent.dim(DIM_HEIGHT) - 1 ) + "]" ) ;

			cursor++ ;

			// Los siguientes elementos son la descripcion de la fila
			register unsigned col = 0;

			while ( col < parent.dim( DIM_WIDTH ) )
			{
				MASSERTMSG( cursor != rowsList.end(), "Insuficient data for initialRow. Last row with " + int2Str(col) + " elements. Note: May be a middle row definition with less elements." ) ;
				string rowVal( (*cursor) );

				nTupla	t;
				t.add(row,col);

				//If the cell is not within the local partition, 
				//nothing will happen
				parent.setCellAllValues( CellPosition(t), str2Real(rowVal) );
				col++ ;
				cursor++;
			}
		}
	}

	if( ini.exists( parent.description(), "initialRowValue" ) )
	{
		MASSERTMSG( parent.dimension().dimension() == 2, "The initialRowValue clause must only be used when the dimension is 2");

		const Ini::IdList &rowsList( ini.definition( parent.description(), "initialRowValue" ) ) ;

		register unsigned row = 0;
		Real val;

		for ( Ini::IdList::const_iterator cursor = rowsList.begin(); cursor != rowsList.end(); cursor++ )
		{
			// the first value is the row number
			row = str2Int( (*cursor) ) ;

			MASSERTMSG( row < parent.dim(DIM_HEIGHT), "The number of row for initialRowValue is out of range. It's " + int2Str(row) + " and must be in [ 0, " + int2Str( parent.dim(DIM_HEIGHT) - 1 ) + "]" ) ;

			cursor++ ;

			MASSERTMSG( cursor != rowsList.end(), "Invalid initial row value for initialRowValue (must be a pair rowNumber rowValues)!" );

			// the second is the description of the row
			string rowVal( *cursor ) ;			

			MASSERTMSG( rowVal.size() == parent.dim(DIM_WIDTH), "The size of the rows for the initial values of the CoupledCell must be equal to the width value !" );

			register unsigned col = 0;

			for (string::iterator rowCurs = rowVal.begin(); rowCurs != rowVal.end(); rowCurs++ )
			{
				if (*rowCurs >= '0' && *rowCurs <= '9')
					val.value( *rowCurs - '0');
				else
					val = Real::tundef;

				nTupla	t;
				t.add( row, col);

				//If the cell is not within the local partition, 
				//nothing will happen
				parent.setCellAllValues( CellPosition(t), val );
				col++;
			}
		}
	}

	if( ini.exists( parent.description(), "initialCellsValue" ) )
		loadInitialCellValuesFromFile( parent, ini.definition(parent.description(), "initialCellsValue").front() );

	if( ini.exists( parent.description(), "initialMapValue" ) )
		loadInitialCellValuesFromMapFile( parent, ini.definition(parent.description(), "initialMapValue").front() );

	return *this ;
}



/*******************************************************************
* Function Name: loadInitialCellValuesFromFile
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadInitialCellValuesFromFile( CoupledCell &parent, const string &fileName )
{
	FILE	*fileIn;
	char	line[250], *posi;
	string	name(trimSpaces(fileName));

	fileIn = fopen( name.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file '" + fileName + "' defined by the initialCellsValue clause");

	while (getLineOnFile(fileIn, line))
		if (line != NULL && (posi = strchr(line, '=')) != NULL)
			parent.setCellAllValues( CellPosition(line), *AbstractValue::from_string(posi+1));

	fclose(fileIn);	
	return *this;
}

/*******************************************************************
* Function Name: loadInitialCellValuesFromMapFile
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadInitialCellValuesFromMapFile( CoupledCell &parent, const string &fileName )
{
	FILE	*fileIn;
	char	line[250];
	string	name(trimSpaces(fileName));

	fileIn = fopen( name.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file '" + fileName + "' defined by the initialMapValue clause");

	CellPosition	counter( parent.dimension().dimension(), 0 );
	register bool	overflow = false;

	while (!overflow)
	{
		MASSERTMSG( getLineOnFile(fileIn, line), "Insuficient data in file specified with InitialMapValue");

		parent.setCellAllValues( counter, str2Real(line) );

		overflow = counter.next( parent.dimension() );
	}
	fclose(fileIn);	
	return *this;
}

/*******************************************************************
* Function Name: loadStateVariables
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadStateVariables( CoupledCell &parent )
{
	string modelName( lowerCase( parent.description() ) ) ;


        // State variables and initial values
	StateVars statevars;
        
	if ( ini.exists( modelName,  "statevariables" ) ) {
		const Ini::IdList &statevariables( ini.definition( modelName, "statevariables" ) ) ;

		if ( ini.exists( modelName,  "statevalues" ) ) {
			const Ini::IdList &initialvalues( ini.definition( modelName, "statevalues" ) ) ;
                        
			Ini::IdList::const_iterator cursor_sv = statevariables.begin();
			Ini::IdList::const_iterator cursor_iv = initialvalues.begin();
			while (cursor_sv != statevariables.end() && cursor_iv != initialvalues.end()) {
				Real value = str2Real( *cursor_iv );
				MASSERTMSG( statevars.createVariable( *cursor_sv, value ),
					    modelName + "/" + "statevariables - Duplicated name: " + *cursor_sv ) ;
				cursor_sv++;
				cursor_iv++;
			}
	                MASSERTMSG( cursor_iv == initialvalues.end() && cursor_sv == statevariables.end(),
		                    modelName + "/" + "statevalues - there must be as many state values as state variables" ) ;
		} else {   // No initial values
			for ( Ini::IdList::const_iterator cursor_sv = statevariables.begin(); cursor_sv != statevariables.end(); cursor_sv++ )
				MASSERTMSG( statevars.createVariable( *cursor_sv ),
					    modelName + "/" + "statevariables - Duplicated name: " + *cursor_sv ) ;
		}
	} else
		MASSERTMSG( !ini.exists( modelName,  "statevalues" ),
		            modelName + "/" + "statevalues - statevariables needed" )

	parent.initialStateVars( statevars );

	return *this ;
}

/*******************************************************************
* Function Name: loadInitialVariablesValues
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadInitialVariablesValues( CoupledCell &parent )
{
	if( ini.exists( parent.description(), "initialVariablesValue" ) )
		loadInitialVariablesValuesFromFile( parent, ini.definition(parent.description(), "initialVariablesValue").front() );

	return *this ;
}

/*******************************************************************
* Function Name: loadInitialVariablesValuesFromFile
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadInitialVariablesValuesFromFile( CoupledCell &parent, const string &fileName )
{
	FILE	*fileIn;
	char	line[250], *posi;
	string	name(trimSpaces(fileName));

	fileIn = fopen( name.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file '" + fileName + "' defined by the initialVariablesValue clause");


	while (getLineOnFile(fileIn, line))
		if (line != NULL && (posi = strchr(line, '=')) != NULL)
			parent.setVariablesValue( CellPosition(line), (const char *) (posi+1) );

	fclose(fileIn);	
	return *this;
}

/*******************************************************************
* Function Name: loadDefaultTransitions
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadDefaultTransitions( CoupledCell &parent, bool printParserInfo )
{
	if( ini.exists( parent.description(), "localTransition" ) )
	{
		parent.localTransition( ini.definition( parent.description(), "localTransition" ).front() ) ;

		registerTransition( parent.localTransition(), printParserInfo );
	}
	return *this;
}

/*******************************************************************
* Function Name: loadPortInTransitions
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadPortInTransitions( CoupledCell &parent, bool printParserInfo )
{
	// Only for CoupledCell and FlatCoupledCell
	if( ini.exists( parent.description(), "portintransition" ) )
	{
		const Ini::IdList &portInTranList( ini.definition( parent.description(), "portintransition" ) ) ;

		// define Cursor
		for( Ini::IdList::const_iterator cursor = portInTranList.begin() ; cursor != portInTranList.end() ; cursor++ )
		{
			string modelAndPortName( *cursor ) ; 
			cursor++;
			MASSERTMSG( cursor != portInTranList.end(), "Invalid portInTransition specification. Second parameter missing!" ) ;

			string actionName( *cursor ), sourcePort, sourceName;

			MASSERTMSG( splitString( modelAndPortName, sourcePort, sourceName, ParallelMainSimulator::separator ), "Invalid portName@cellModelName in portInTransition" );

			////////////////////////////////////////////////////
			// get the ELSE statement
			string elseFunction( "" );

			if( ini.exists( actionName,  "else" ) ){
				elseFunction = ini.definition( actionName, "else" ).front() ;

				MASSERTMSG( elseFunction != actionName, "The Else Function '" + elseFunction + "' can't be the same as the portInFunction.\nProbably it will generate an infinite loop." );

				string	ef = elseFunction, elseEF;

				while (ef != "")
				{
					elseEF = "";
					if( ef != "" && ini.exists( ef,  "else" ) )
						elseEF = ini.definition( ef, "else" ).front() ;

					MASSERTMSG( ef != elseEF, "The Else Function '" + ef + "' can't be the same as the portInFunction.\nProbably it will generate an infinite loop." );

					registerTransitionPortIn( ef, printParserInfo, elseEF );
					ef = elseEF;
				}
			}
			//////////////////////////////////////////////////////

			// parse the function definition
			registerTransitionPortIn( actionName, printParserInfo, elseFunction );

			// NOTE: If the port not exist, the setPortInFunction will produce an exception.

			// Now, make an association between the function and the input port
			parent.setPortInFunction( sourceName, sourcePort, actionName );
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadLocalZones
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::loadLocalZones( CoupledCell &parent, bool printParserInfo )
{
	if( ini.exists( parent.description(), "zone" ) )
	{
		const Ini::IdList &zoneList( ini.definition( parent.description(), "zone" ) ) ;

		// zone Cursor
		for( Ini::IdList::const_iterator zoneCurs = zoneList.begin() ; zoneCurs != zoneList.end() ; zoneCurs++ )
		{
			string fname( *zoneCurs ) ; 
			registerTransition( fname, printParserInfo );

			zoneCurs++ ;

			MASSERTMSG( zoneCurs != zoneList.end() && *zoneCurs == "{", "Invalid zone specification, '{' missing!" ) ;
			zoneCurs++ ;

			while( zoneCurs != zoneList.end() && *zoneCurs != "}" )
			{
				Zone zone( *zoneCurs );
				for( Zone::Iterator cursor = zone.begin(); cursor != zone.end(); cursor++ )
					parent.setLocalTransition( *cursor, fname );

				zoneCurs++;
			}

			MASSERTMSG( zoneCurs != zoneList.end() && *zoneCurs == "}", "Invalid zone specification, '}' missing!" ) ;
		} // zone Cursor
	}
	return *this ;
}

/*******************************************************************
* Method: registerTransition
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::registerTransition(const LocalTransAdmin::Function &fName, bool printParserInfo )
{
	if( !SingleLocalTransAdmin::Instance().isRegistered( fName ) )
	{
		if( ini.exists( fName ) && ini.exists( fName, "rule" ) )
		{
			const Ini::IdList &fnList( ini.definition( fName, "rule" ) ) ;

			strstream buffer ;
			for( Ini::IdList::const_iterator cursor = fnList.begin() ; cursor != fnList.end() ; cursor++ )
				buffer << " " << *cursor ;

			if( printParserInfo == true )
			{
				ParserDebug().Stream() << "********* BUFFER ********" << endl ;
				ParserDebug().Stream().write(buffer.str(), buffer.pcount() );
				ParserDebug().Stream() << endl ;
			}

			SingleLocalTransAdmin::Instance().registerTransition( fName, buffer, printParserInfo, false) ;
		} else
		{
			MException e( string( "The local transition " ) + fName + " is missing in the ini file!" ) ;
			MTHROW( e ) ;
		}
	}
	return *this;
}

/*******************************************************************
* Method: registerTransitionPortIn
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::registerTransitionPortIn(const LocalTransAdmin::Function &fName, bool printParserInfo, const string &elseFunction )
{
	if( !SingleLocalTransAdmin::Instance().isRegistered( fName ) )
	{
		if( ini.exists( fName ) && ini.exists( fName, "rule" ) )
		{
			const Ini::IdList &fnList( ini.definition( fName, "rule" ) ) ;

			strstream buffer ;
			for( Ini::IdList::const_iterator cursor = fnList.begin() ; cursor != fnList.end() ; cursor++ )
				buffer << " " << *cursor ;

			if( printParserInfo == true )
			{
				ParserDebug().Stream() << "********* BUFFER (for PortIn Transition) ********" << endl ;
				ParserDebug().Stream().write( buffer.str(), buffer.pcount() ) ;
				ParserDebug().Stream() << endl ;
			}
			SingleLocalTransAdmin::Instance().registerTransition( fName, buffer, printParserInfo, true, elseFunction) ;
		} else
		{
			MException e( string( "The local transition " ) + fName + " is missing in the ini file!" ) ;
			MTHROW( e ) ;
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: showEvents
********************************************************************/
void ParallelMainSimulator::showEvents( const EventList &events, ostream &out  )
{
	for (EventList::const_iterator cursor = events.begin() ; cursor != events.end() ; cursor++ )
		out << cursor->asString() << endl;
}

/*******************************************************************
* Function Name: showLogInfo
* Lists all the log files associated to each model. The output will
* be used by drawlog.
********************************************************************/
void ParallelMainSimulator::showLogInfo( ostream &out )
{

	const ParallelModelAdmin::ModelDB& models( SingleParallelModelAdm::Instance().models());

	out << "[logfiles]" << endl;

	for (ParallelModelAdmin::ModelDB::const_iterator cursor = models.begin() ;
	cursor != models.end(); cursor++) {

		Model* model = cursor->second;

		out << model->description() << " : ";

		for(ModelPartition::const_iterator mcursor = model->modelPartition().begin();
		mcursor != model->modelPartition().end(); mcursor++) {

			string logfilename(logName() + mcursor->second);

			out << logfilename << " ";

		}

		out << endl;
	}

}

/*******************************************************************
* Function Name: setUpLogicalProcess
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::setUpLogicalProcess()
{

	ostream& out = debugStream();

	const ParallelModelAdmin::ModelDB& models(SingleParallelModelAdm::Instance().models());

	//Create a new Logical Process.
	//Remember that in addition to all the model objects, there exists
	//the Root Processor, a Warped object!

	out << "Setting up the logical process " << endl;

	int totalObjectsCount = SingleParallelModelAdm::Instance().totalObjectsCount();
	int localObjectsCount = SingleParallelModelAdm::Instance().localObjectsCount();

	out << "Total objects: " << totalObjectsCount << endl;
	out << "Local objects: " << localObjectsCount << endl;
	out << "Total machines: " << SingleParallelModelAdm::Instance().totalMachinesCount() << endl;
	out << endl;
	out << "About to create the LP" << endl;

	lp = new LogicalProcess(totalObjectsCount, 
			localObjectsCount,
			SingleParallelModelAdm::Instance().totalMachinesCount());

	out << "LP has been created. Now registering processors." << endl;

	//Now, register all DEVS processors that are going to run in this LP.

	for (ParallelModelAdmin::ModelDB::const_iterator cursor = models.begin() ;
	cursor != models.end(); cursor++) {

		Model* model = cursor->second;

		//If the model has a local processor on this LP
		if(model->localProc() != ParallelProcessor::InvalidId) {
			ParallelProcessor &proc = model->createParallelProcessor();
			lp->registerObject(&proc);
			out << "\tRegistering processor " << proc.description() << "(" << proc.id() << ")" << endl;
		}

	}

	return *this;
}

/*******************************************************************
* Function Name: afterInitialize
********************************************************************/
ParallelMainSimulator &ParallelMainSimulator::afterInitialize()
{

	const ParallelModelAdmin::ModelDB& models(SingleParallelModelAdm::Instance().models());

	for (ParallelModelAdmin::ModelDB::const_iterator cursor = models.begin() ;
	cursor != models.end(); cursor++) {

		Model* model = cursor->second;

		//If the model has a local processor on this LP
		if(model->localProc() != ParallelProcessor::InvalidId) {
			model->afterProcessorInitialize();
		}

	}

	return *this;
}

/*******************************************************************
* Function Name: showModelPatition
********************************************************************/
void ParallelMainSimulator::showModelPartition(ostream &out)
{
	const ParallelModelAdmin::ModelDB& models( SingleParallelModelAdm::Instance().models());

	for (ParallelModelAdmin::ModelDB::const_iterator cursor = models.begin() ;
	cursor != models.end(); cursor++) {

		Model* model = cursor->second;

		out << "Model: " << model->description() << endl;
		out << "\tMachines: " << endl;;

		for(ModelPartition::const_iterator mcursor = model->modelPartition().begin();
		mcursor != model->modelPartition().end(); mcursor++) {
			out << "\t\t Machine: " << mcursor->first << "\tProcId: " << mcursor->second;

			if ( mcursor->second == model->localProc() )
				out << " < local > "; 

			if ( mcursor->second == model->masterId())
				out << " < master > ";

			out << endl;
		}

		out << endl;
	}
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

int getLineOnFile(FILE *fileIn, char *linea)
{
#define	FILE_BUFFER_SIZE 25000
	static	char	buffer[FILE_BUFFER_SIZE];
	static	long	leido = FILE_BUFFER_SIZE;
	static	long	porLeer = FILE_BUFFER_SIZE;
	register	bool	encontre = false;
	register	long	pos = 0;

	while (!encontre)
	{
		if (leido == FILE_BUFFER_SIZE || porLeer == 0)
		{
			if (feof(fileIn) || (porLeer = fread( buffer, 1, FILE_BUFFER_SIZE, fileIn )) == 0)
			return 0;

			leido = 0;
		}

		if (buffer[leido] != '\n')
			linea[pos++] = buffer[leido++];
		else
		{
			encontre = true;
			leido++;
		}

		porLeer--;
	}
	linea[pos] = 0;
	return 1;
}

/*******************************************************************
* Function Name: debugStream
********************************************************************/

ostream& ParallelMainSimulator::debugStream() {



	if ( (!vDebugStream) ) 
	{
		if ( loader()->partitionDebugName() != "/dev/null" ) 
		{
			string filename = loader()->partitionDebugName() + getMachineID();
			vDebugStream = new fstream( filename.c_str(), ios::out);
		} 
		else
		{
			vDebugStream = new fstream( "/dev/null", ios::out);
		}
	}

	return *vDebugStream;
}
