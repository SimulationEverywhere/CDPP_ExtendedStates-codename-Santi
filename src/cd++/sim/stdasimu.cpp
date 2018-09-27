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

/** include files **/
#include <fstream>
#include <strstream>
#include "stdasimu.h"	// base header
#include "sprocadm.h"	// SingleStandAloneProcessorAdmin
#include "ini.h"        	// class Ini
#include "pmodeladm.h"   	// class SingleParallelModelAdm
#include "strutil.h"    	// lowerCase, str2Int
#include "coupled.h"    	// class Coupled
#include "zone.h"       	// class zone
#include "flatcoup.h"   	// class FlatCoupledCell
#include "tdcell.h"		//
#include "idcell.h"		//

using namespace std;

/*******************************************************************
* Function Name: run
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::run()
{
	if( !loader() )
	{
		MException e( "The ParallelMainSimulator loader not found!" ) ;
		e.addText( "The loader must be seted before running the simulation." ) ;
		MTHROW( e ) ;
	}
	
	loader()-> loadData();
	
	loadModels( loader()->modelsStream(), loader()->printParserInfo() );
	
	loadExternalEvents( loader()->eventsStream() );
	
	Root::Instance().stopTime( loader()->stopTime() );
	
	Root::Instance().simulate();
	
	loader()->writeResults();
	
	return *this;
}

/*******************************************************************
* Function Name: loadModels
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadModels( istream &file, bool printParserInfo )
{
	ini.parse( file );
	
	loadModel( Root::Instance().top(), iniFile(), printParserInfo );
	
	return *this;
}


/*******************************************************************
* Function Name: loadModel
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadModel( Coupled &parent, Ini &ini, bool printParserInfo )
{
	// cargamos los ports
	loadPorts( parent, ini );
	
	// los componentes
	if( parent.type() == Coupled::cell )
		loadCells( static_cast<CoupledCell&>( parent ), ini, printParserInfo ) ;
	else
		loadComponents( parent, ini, printParserInfo ) ;
	
	// los links
	loadLinks( parent, ini ) ;
	
	// At this time, the links are created. Now, we load the portInTransitions
	if ( parent.type() == Coupled::cell)
		loadPortInTransitions( static_cast<CoupledCell&>(parent), ini, printParserInfo ) ;
	
	return *this;
}

/*******************************************************************
* Function Name: loadPorts
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadPorts( Coupled &parent, Ini &ini )
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
ParallelMainSimulator &StandAloneMainSimulator::loadComponents( Coupled &parent, Ini &ini, bool printParserInfo )
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
					else if( type == "flat" )
						coupled = &( SingleParallelModelAdm::Instance().newFlatCoupledCell( mName ) ) ;
					else
						MASSERTMSG( false, "Invalid coupled type " + type + "!" ) ;
				} else
					coupled = &( SingleParallelModelAdm::Instance().newCoupled( mName ) ) ;
				
				parent.addModel( *coupled ) ;
				
				loadModel( *coupled, ini, printParserInfo ) ;
			}
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadCells
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadCells( CoupledCell &parent, Ini &ini, bool printParserInfo )
{
	string modelName( lowerCase( parent.description() ) ) ;
	
	// get the width and height
	if ( ini.exists( modelName,  "dim" ) )
	{
		MASSERTMSG( !ini.exists( modelName, "width" ) && !ini.exists( modelName, "height" ), "The DIM clause can't coexist with the WITDH or the HEIGHT clauses");
		Ini::IdList	dimension = ini.definition( modelName, "dim" );
		CellPosition	cp( ini.join(dimension) );
		nTupla		dims( cp );
		
		MASSERTMSG( dims.dimension() >= 2, "The tuple defined with the DIM clause is invalid. It's must have 2 or more elements.");
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
	
	Real initialValue( str2Real( ini.definition( modelName, "initialvalue" ).front() ) ) ;
	parent.initialCellValue( initialValue ) ;
	
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
	
	loadDefaultTransitions( parent, ini, printParserInfo ) ;
	
	CellPositionList selectList ;
	if( ini.exists( modelName,  "select" ) )
	{
		const Ini::IdList &select( ini.definition( modelName, "select" ) ) ;
		for( Ini::IdList::const_iterator cursor = select.begin() ; cursor != select.end() ; cursor++ )
			selectList.push_back( CellPosition( *cursor ) ) ; 
	}
	
	parent.createCells( neighborsList, selectList ) ;
	
	loadInitialCellValues( parent, ini ) ;
	
	loadLocalZones( parent, ini, printParserInfo ) ;
	
	return *this ;
}

/*******************************************************************
* Function Name: loadLinks
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadLinks( Coupled &parent, Ini &ini )
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
			
			////////////////////////////////////////////////////////
			// If the model is of type CoupledCell and the input
			// port don't exist, then I create it.
			if (parent.className() != FLAT_COUPLED_CELL_NAME)
			{
				string modelName = SingleStandAloneProcessorAdmin::Instance().processor( destName ).model().className();
				
				if ( modelName == TRANSPORT_DELAY_CELL_NAME || modelName == INERTIAL_DELAY_CELL_NAME ) 
					// the addInPort method adds a port if not exist
					static_cast<AtomicCell*>(&SingleStandAloneProcessorAdmin::Instance().processor( destName).model())->addInPort( destPort );
			}
			// NOTE: If the parent is a FlatCoupledCell, then addInfluence will create the port
			///////////////////////////////////////////////////////
			
			////////////////////////////////////////////////////////
			// If the model is of type CoupledCell and the output
			// port don't exist, then I create it.
			if (parent.className() != FLAT_COUPLED_CELL_NAME)
			{
				string modelName = SingleStandAloneProcessorAdmin::Instance().processor( sourceName ).model().className();
				
				if ( modelName == TRANSPORT_DELAY_CELL_NAME || modelName == INERTIAL_DELAY_CELL_NAME ) 
					// the addOutPort method adds a port if not exist
					static_cast<AtomicCell*>(&SingleStandAloneProcessorAdmin::Instance().processor(sourceName).model())->addOutPort( sourcePort );
			}
			// NOTE: If the parent is a FlatCoupledCell, then addInfluence will create the port
			///////////////////////////////////////////////////////
			
			parent.addInfluence( sourceName, sourcePort, destName, destPort ) ;
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadExternalEvents
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadExternalEvents( istream  &fileIn )
{
	Root::Instance().initialize() ;
	
	istream_iterator<string> cursor( fileIn ) ;
	
	try
	{
		while( cursor != istream_iterator<string>() )
		{
			VTime time( *cursor ) ;
			cursor ++ ;
			
			Port &port( Root::Instance().top().port( *cursor ) ) ;
			cursor ++ ;
			
			Root::Instance().addExternalEvent( time, port, str2Real( *cursor ) ) ;
			cursor++ ;
		} 
	} catch( InvalidPortRequest &e )
	{
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		throw e ;
	}
	this->showEvents( Root::Instance().events() ) ;
	
	return *this;
}

/*******************************************************************
* Function Name: loadInitialCellValues
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadInitialCellValues( CoupledCell &parent, Ini &ini )
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
				
				parent.setCellValue( CellPosition(t), str2Real(rowVal) );
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
				
				parent.setCellValue( CellPosition(t), val );
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
* Function Name: loadInitialCellValuesFromFile
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadInitialCellValuesFromFile( CoupledCell &parent, const string &fileName )
{
	FILE	*fileIn;
	char	line[250], *posi;
	string  file(trimSpaces(fileName));
	
	fileIn = fopen( file.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file '" + fileName + "' defined by the initialCellsValue clause");
	
	while (getLineOnFile(fileIn, line))
		if (line != NULL && (posi = strchr(line, '=')) != NULL)
			parent.setCellValue( CellPosition(line), str2Real(posi+1) );
		
	fclose(fileIn);	
	return *this;
}

/*******************************************************************
* Function Name: loadInitialCellValuesFromMapFile
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadInitialCellValuesFromMapFile( CoupledCell &parent, const string &fileName )
{
	FILE	*fileIn;
	char	line[250];
	string  file(trimSpaces(fileName));
	
	fileIn = fopen( file.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file '" + fileName + "' defined by the initialMapValue clause");
	
	CellPosition	counter( parent.dimension().dimension(), 0 );
	register bool	overflow = false;
	
	while (!overflow)
	{
		MASSERTMSG( getLineOnFile(fileIn, line), "Insuficient data in file specified with InitialMapValue");
		
		parent.setCellValue( counter, str2Real(line) );
		
		overflow = counter.next( parent.dimension() );
	}
	fclose(fileIn);	
	return *this;
}

/*******************************************************************
* Function Name: loadDefaultTransitions
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadDefaultTransitions( CoupledCell &parent, Ini &ini, bool printParserInfo )
{
	if( ini.exists( parent.description(), "localTransition" ) )
	{
		parent.localTransition( ini.definition( parent.description(), "localTransition" ).front() ) ;
		
		this->registerTransition( parent.localTransition(), ini, printParserInfo );
	}
	return *this;
}

/*******************************************************************
* Function Name: loadPortInTransitions
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadPortInTransitions( CoupledCell &parent, Ini &ini, bool printParserInfo )
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
					
					registerTransitionPortIn( ef, ini, printParserInfo, elseEF );
					ef = elseEF;
				}
			}
			//////////////////////////////////////////////////////
			
			// parse the function definition
			registerTransitionPortIn( actionName, ini, printParserInfo, elseFunction );
			
			// NOTE: If the port not exist, the setPortInFunction will produce an exception.
			
			// Now, make an association between the function and the input port
			if (parent.className() == COUPLED_CELL_NAME)
				static_cast<AtomicCell*>(&SingleStandAloneProcessorAdmin::Instance().processor(sourceName).model())->setPortInFunction( sourcePort, actionName );
			else if (parent.className() == FLAT_COUPLED_CELL_NAME)
				static_cast<FlatCoupledCell*>(&parent)->setPortInFunction( sourceName, sourcePort, actionName );
			else
				MASSERTMSG( false, "Can't set the PortIn Function in a model of type " + parent.className() );
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: loadLocalZones
********************************************************************/
ParallelMainSimulator &StandAloneMainSimulator::loadLocalZones( CoupledCell &parent, Ini &ini, bool printParserInfo )
{
	if( ini.exists( parent.description(), "zone" ) )
	{
		const Ini::IdList &zoneList( ini.definition( parent.description(), "zone" ) ) ;
		
		// zone Cursor
		for( Ini::IdList::const_iterator zoneCurs = zoneList.begin() ; zoneCurs != zoneList.end() ; zoneCurs++ )
		{
			string fname( *zoneCurs ) ; 
			registerTransition( fname, ini, printParserInfo );
			
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
ParallelMainSimulator &StandAloneMainSimulator::registerTransition(const LocalTransAdmin::Function &fName, Ini &ini, bool printParserInfo )
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
ParallelMainSimulator &StandAloneMainSimulator::registerTransitionPortIn(const LocalTransAdmin::Function &fName, Ini &ini, bool printParserInfo, const string &elseFunction )
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
void StandAloneMainSimulator::showEvents( const EventList &events, ostream &out  )
{
	for (EventList::const_iterator cursor = events.begin() ; cursor != events.end() ; cursor++ )
		out << cursor->asString() << endl;
}
