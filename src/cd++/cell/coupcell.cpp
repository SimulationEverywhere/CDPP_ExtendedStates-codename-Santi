/*******************************************************************
*
*  DESCRIPTION: class CoupledCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 06/03/1999 (v2)
*  DATE: 12/02/2001 (v3)
*
*******************************************************************/

/** include files **/
#include <algorithm>
#include "coupcell.h"         // CoupledCell
#include "strutil.h"          // class int2Str
#include "atomcell.h"         // class AtomicCell
#include "pprocadm.h"         // SingleParallelProcessorAdmin
#include "pmodeladm.h"        // SingleParallelModelAdm
#include "cellpartition.h"	  //CellPartition
#include "idcell.h"
#include "tdcell.h"
#include "except.h"

using namespace std;

/** methods **/

CoupledCell::CoupledCell( const string &name )
	: Coupled( name )
	, partition ( NULL )
	, localFn( LocalTransAdmin::InvalidFn )
{}

/*******************************************************************
* Function Name: ~CoupledCell
********************************************************************/
CoupledCell::~CoupledCell()
{

	if (partition )
		delete partition;
}


/*******************************************************************
* Function Name: addInfluence
* Description: add a link between two ports
********************************************************************/
Model &CoupledCell::addInfluence( const string &sourceName, const string &sourcePort, const string &destName, const string &destPort) 
{

	Model& source = SingleParallelModelAdm::Instance().model( sourceName );
	Model& destination = SingleParallelModelAdm::Instance().model( destName );

	//If the either modelName is the name of the coupled, the port
	//has already been created. If the model is a cell, the port
	//must be created.
	if ( sourceName != description() )
		static_cast< AtomicCell& >(source).addOutPort( sourcePort );

	if ( destName != description() )
		static_cast< AtomicCell& >(destination).addInPort( destPort );

	Port &port1( source.port( sourcePort ) );
	Port &port2( destination.port( destPort ) );

	port1.addInfluence( port2 );


	return *this;
}

/*******************************************************************
* Function Name: afterProcessorInitialize
********************************************************************/
void CoupledCell::afterProcessorInitialize() 
{ 
	//Two things must be done now that we have an initial state:
	//1. Create all the local cell's neighborhoods and set the
	//   default values

	PModelList::const_iterator childrenCursor;

	for ( childrenCursor = childModels().begin();
	      childrenCursor != childModels().end();
	      childrenCursor++) {

		if( (*childrenCursor)->localProc() != ParallelProcessor::InvalidId )
		{
			AtomicCell* cell = (AtomicCell*)(*childrenCursor);
			cell->initializeCell();
			cell->setAllNCPortsValues( initialCellValue());
                        cell->variables( initialStateVars() );
		}
	}

	//2. Load the initial cell values.
	ParallelMainSimulator::Instance().loadInitialCellValues( *this );
	ParallelMainSimulator::Instance().loadInitialVariablesValues( *this );

}

/*******************************************************************
* Function Name: createCells
********************************************************************/
CoupledCell &CoupledCell::createCells( const CellPositionList &neighborList, 
                                       CellPartition *part,
				       std::list<string> NCPorts)
{
	unsigned long cellIndex;

	MASSERTMSG( !dimension().contains(0), "Attemp to create a Coupled Cell Model with a dimension containing the value 0.");

	partition = part;

	neighbors = neighborList;

	AtomicCell **matrix = new AtomicCell*[ dimension().totalElements() ] ;

	//Set all the pointers to NULL. This will help to check if the cell has 
	//already been created.
	for ( long i = 0; i < dimension().totalElements(); i++ ) {
		matrix[i] = NULL;
	}

	//////////////////////
	// cell creation
	//////////////////////
	AtomicCell *atomicCell;
	CellPositionList::const_iterator cursor;

	CellPosition 	counter( dimension().dimension(),0);
	register bool	overflow = false;
	

	while ( !overflow ){

		cellIndex =  counter.calculateIndex( dimension() );

		//Right now, I have decided to create all the models
		//Later, only does models that are required will be created

		if (matrix [cellIndex] == NULL)		// no encontre
			matrix[ cellIndex ] = &( SingleParallelModelAdm::Instance().newAtomicCell( counter, inertial, cellName(counter) ) );

		atomicCell = matrix[ cellIndex ];

		atomicCell->localFunction( localTransition() );
		
		atomicCell->createNCPorts( NCPorts );

		// inserts ordered by id
		addModel( *atomicCell );

		overflow = counter.next( dimension() );
	}//while

	/////////////////////
	// links creation
	/////////////////////
	counter.fill( dimension().dimension(), 0 );
	overflow = false;

	CellPosition auxi;
	unsigned long cellIndex2;
	list<string>::iterator cursor2;

	while ( !overflow )
	{
		cellIndex = counter.calculateIndex( dimension() );

		MASSERTMSG( matrix[ cellIndex ] != NULL, "Incorrect value for matrix in CoupledCell::CreateCells");

		for (cursor = neighbors.begin(); cursor != neighbors.end(); cursor++)
		{
			// get the real position
			auxi = *cursor;
			auxi += counter;

			if ( borderWrapped() )
				auxi.canonizar( dimension() );

			if ( dimension().includes( auxi ) ) {
				cellIndex2 =  auxi.calculateIndex( dimension() );

				// Add the neighbor port
				matrix[ cellIndex2 ]->port( AtomicCell::outPort ).addInfluence(
						matrix[ cellIndex ]->port( AtomicCell::neighborChangePort ) );

				// Add the other neighbor change ports
				for (cursor2 = matrix[ cellIndex2 ]->NCPorts().begin();
				     cursor2 != matrix[ cellIndex2 ]->NCPorts().end();
				     cursor2++) {
					matrix[ cellIndex2 ]->port( AtomicCell::NCOutPrefix + *cursor2 ).addInfluence(
							matrix[ cellIndex ]->port( AtomicCell::NCInPrefix + *cursor2 ) );
				}

				//addInfluence( matrix[ cellIndex2 ]->description(), AtomicCell::outPort,
				//matrix[ cellIndex ]->description(), AtomicCell::neighborChangePort );
			}
		}

		overflow = counter.next( dimension() );
	}

	delete [] matrix;
	return *this;

}

/*******************************************************************
* Method: localTransition
********************************************************************/
CoupledCell &CoupledCell::setLocalTransition( const CellPosition &pos, const LocalTransAdmin::Function &fName )
{
	//Attempt to register the transition.
	//It might be the case the cell does not belong to the partition.
	//In that case, do nothing

	unsigned long cellIndex;
	cellIndex = cellPartition().cellIndex( pos );

	if( cellIndex != CellPartition::InvalidCellIndex ) {
		AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( cellName( pos ) ) ) );
		cell.localFunction( fName );

	}

	return *this ;
}

/*******************************************************************
* Method: setCellMachine
********************************************************************/
CoupledCell& CoupledCell::setCellMachine( const CellPosition &pos, const MachineId& m, const ProcId& p) 
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( cellName( pos ) ) ) );
	cell.setMachineProcId( m , p);;

	//Set the parent/child relationship
	if ( cellPartition().localCellsInclude( pos ) ) {
		cell.parentId( localProc() );
	} else {
		cell.parentId( procInMachine(m) );	
	}

	return *this ;

}
/*******************************************************************
* Method: setPortInFunction
********************************************************************/
void CoupledCell::setPortInFunction( const CellPosition &pos, const string &sourcePort, const string &actionName )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( cellName( pos ) ) ) );
	cell.setPortInFunction( sourcePort, actionName );	;
}

/*******************************************************************
* Function Name: setCellValue
* This function will fail if the cell does not have a local processor
********************************************************************/
CoupledCell &CoupledCell::setCellValue( const ModelId &model, const string &port, Value v )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( model ) ) );

	cell.value( Real(v), port );
	return *this ;
}

/*******************************************************************
* Function Name: setCellValue
********************************************************************/
CoupledCell &CoupledCell::setCellValue( const CellPosition &pos, const string &port, const Real &v )
{
	if ( cellPartition().localCellsInclude( pos ) )
	{
		string sCellPos = cellName( pos );
		setCellValue( sCellPos, port, v);
	}	
	return *this;
}

/*******************************************************************
* Function Name: setCellValue
* This function will fail if the cell does not have a local processor
********************************************************************/
CoupledCell &CoupledCell::setCellValue( const string &sCellPos, const string &port, const Real &v )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( sCellPos ) ) );
	cell.value( v, port );

	return *this ;
}

/*******************************************************************
* Function Name: setCellAllValues
* This function will fail if the cell does not have a local processor
********************************************************************/
CoupledCell &CoupledCell::setCellAllValues( const ModelId &model, Value v )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( model ) ) );

	cell.setAllNCPortsValues( Real(v) );
	return *this ;
}

/*******************************************************************
* Function Name: setCellAllValues
********************************************************************/
CoupledCell &CoupledCell::setCellAllValues( const CellPosition &pos, const AbstractValue &v )
{
	if ( cellPartition().localCellsInclude( pos ) )
	{
		string sCellPos = cellName( pos );
		setCellAllValues( sCellPos, v);
	}	
	return *this;
}

/*******************************************************************
* Function Name: setCellAllValues
* This function will fail if the cell does not have a local processor
********************************************************************/
CoupledCell &CoupledCell::setCellAllValues( const string &sCellPos, const AbstractValue &v )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( sCellPos ) ) );
	cell.setAllNCPortsValues( v );

	return *this ;
}

/*******************************************************************
* Function Name: setVariablesValue
********************************************************************/
CoupledCell &CoupledCell::setVariablesValue( const CellPosition &pos, const char *v )
{
	if ( cellPartition().localCellsInclude( pos ) )
	{
		string sCellPos = cellName( pos );
		setVariablesValue( sCellPos, v );
	}	
	return *this;
}

/*******************************************************************
* Function Name: setVariablesValue
* This function will fail if the cell does not have a local processor
********************************************************************/
CoupledCell &CoupledCell::setVariablesValue( const string &sCellPos, const char *v )
{
	AtomicCell &cell( static_cast< AtomicCell & >( SingleParallelModelAdm::Instance().model( sCellPos ) ) );
	cell.variables().setValues( string( v ) );

	return *this ;
}

/*******************************************************************
* Function Name: totalProcCount
********************************************************************/
unsigned long CoupledCell::totalProcCount() const
{
	long count = 0;

	count = dimension().totalElements();
	count += modelPartition().size();

	return count;
}

/*******************************************************************
* Function Name: localProcCount
********************************************************************/
unsigned long CoupledCell::localProcCount() const 
{
	long count = 0;

	count = cellPartition().totalLocalElements();
	if ( localProc() != ParallelProcessor::InvalidId )
		count++;

	return count;
}

/*******************************************************************
* Function Name: inverseNeighborhood
********************************************************************/
CellPosition CoupledCell::inverseNeighborhood( const CellPosition& center, const CellPosition& other) const
{

	if ( !borderWrapped() )
	{
		CellPosition nPos = other;
		nPos -= center;

		return nPos;	
	}
	else
	{
		//if the border is wrapped, check each neighbor
		for( CellPositionList::const_iterator cursor = neighborhood().begin();
		cursor != neighborhood().end(); cursor++)
		{

			CellPosition nPos = *cursor;
			nPos += center;
			nPos.canonizar( dimension() );

			if (nPos == other)
				return *cursor;
		}//for
	}//if

	MException e;
	e.addText( "inverseNeighborhood: No neighbor found!");
	MTHROW ( e );
}
