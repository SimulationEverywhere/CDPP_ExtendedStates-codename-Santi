/*******************************************************************
*
*  DESCRIPTION: class LocalTransitionAdmin and SingleLocalTransitionAdmin
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 21/04/1998 (v2)
*
*******************************************************************/

// ** include files **//
#include "ltranadm.h"     // header class
#include "parser.h"
#include "neighval.h"     // NeighborhoddValuef
#include "strutil.h"      // lowerCase
#include "synnode.h"      // SpecNode
#include "evaldeb.h"	  // EvalDebug
#include "atomcell.h"
#include "value.h"

using namespace std;

// ** public data ** //
const LocalTransAdmin::Function LocalTransAdmin::InvalidFn ;

// ** private data ** //             
LocalTransAdmin *SingleLocalTransAdmin::instance( NULL ) ;

/*******************************************************************
* Function Name: ~LocalTransAdmin() 
********************************************************************/
LocalTransAdmin::~LocalTransAdmin()
{
	cout << "Bye Bye" << endl;

	for (LocalTransDB::iterator cursor = this->transDB.begin(); cursor != this->transDB.end(); cursor++ )
		delete cursor->second;
}

/*******************************************************************
* Method: LocalTransAdmin
********************************************************************/
LocalTransAdmin::LocalTransAdmin() 
	: symbolTable( NULL )
{}

/*******************************************************************
* Method: evaluate
********************************************************************/
list<PortValue> LocalTransAdmin::evaluate( const Function &fn,
                                	   const NeighborhoodValue &neighbors,
                                	   PortValues *portsVal,
                                	   VTime &delayTime,
                                	   VTime &actualtime,
                                	   VirtualPortList *outPorts,
                                	   Model *actModel,
                                	   string portSource )
{
	Real	        valueReturned, dt;
	list<PortValue> lastValidValue;

	this->symbolTable = &neighbors;
	this->portTable = portsVal;
	this->ActualTime = actualtime;
	this->functionname = fn;
	this->portsource = portSource;
	this->outPortList = outPorts;
	this->evalModel = actModel;

	MASSERTMSG( this->transDB.find( fn ) != this->transDB.end(), "Funcion '" + fn + "' Not Found!");

	SpecNode *spec( (*this->transDB.find( fn )).second );

	// Throws an exception when two rules evaluate to true ( and the result or the delay is different) 
	// or no rule is satisfied!!!!
	try
	{
		if (EvalDebug().Active()) {
			EvalDebug().Stream() << "+---------------------------------------------------------------------------+\n"
			                     << "New Evaluation: " << fn << " - " << actualtime.asString() << " - " << actModel->description() << "\n";
			EvalDebug().Stream() << "StateVars: " << ((AtomicCell *) actModel)->variables().asString() << "\n";
		}

		spec->resetWarningCondition();
		valueReturned = Real::from_value(spec->evaluate());

		// If there isn't any TRUE rule and exist an ElseFunction
		// then evaluate it.
		string elseFunction = spec->elseFunction();
		dt = spec->delay();
		lastValidValue = spec->value();
		register bool anyUndef =  spec->AnyUndef();
		register bool anyStochastic = spec->AnyStochastic();

		while (valueReturned.value() != 1 && elseFunction != "")
		{
			MASSERTMSG( this->transDB.find( elseFunction ) != this->transDB.end(), "Funcion '" + elseFunction + "' Not Found!\n Abort, Retry or Pizza?" );
			SpecNode *spec2( (*this->transDB.find( elseFunction )).second ) ;

			valueReturned = spec2->evaluate( anyUndef, anyStochastic );

			elseFunction = spec2->elseFunction();
			dt = spec2->delay();
			lastValidValue = spec->value();
			anyUndef =  spec2->AnyUndef();
			anyStochastic = spec2->AnyStochastic();
		}

		if (EvalDebug().Active()) {
			EvalDebug().Stream() << "StateVars: " << ((AtomicCell *) actModel)->variables().asString() << "\n";
		}
	} catch( InvalidEvaluation &e )
	{
		e.addText( "Language used is: " + fn );
		e.addNeighborhood( neighbors );
		e.print(cerr);

		MTHROW( e );
	}

	// If the evaluation is OK (any evaluate to TRUE) then set the correct delay time
	if ( valueReturned == Real::one )
	{
		delayTime = VTime::Zero ;

		MASSERTMSG( !dt.IsUndefined(), "The delay time of a rule is undefined (?)" );

		delayTime.mseconds( dt.value() ) ;
	}
	// else, the delay time is the default delay time.

	return lastValidValue;
}

/*******************************************************************
* Function Name: register
********************************************************************/
LocalTransAdmin &LocalTransAdmin::registerTransition( const string &transName, istream &transText, bool printParserInfo, bool parseForPortIn, const string &elseFunction )
{
	try
	{
		SingleParser::Instance().parse( transText, printParserInfo, parseForPortIn, elseFunction ) ;
	} catch( MException &e )
	{
		e.addLocation( MEXCEPTION_LOCATION() );
		e.addText( "Error parsing " + transName );
		throw;
	}

	MASSERTMSG( !isRegistered( transName ), transName + " exist in the function DB!" ) ;
	transDB[ lowerCase( transName ) ] = SingleParser::Instance().specification() ;

	return *this ;
}

/*******************************************************************
* Method: cellValue
********************************************************************/
const AbstractValue &LocalTransAdmin::cellValue( const NeighborPosition &pos, const string PortName )
{
	MASSERT( this->symbolTable ); 
	
	string name = lowerCase(PortName);
	
	if (name != lowerCase(AtomicCell::neighborChangePort) &&
	    name.find(AtomicCell::NCInPrefix) != 0)
		name = AtomicCell::NCInPrefix + name;
	
	return this->symbolTable->get(pos, name);
}

/*******************************************************************
* Method: isRegistered
********************************************************************/
bool LocalTransAdmin::isRegistered( const Function &name ) const
{
	return this->transDB.find( lowerCase( name ) ) != this->transDB.end() ;
}

/*******************************************************************
* Method: portValue
* Description: return the value associated with a InPort
********************************************************************/
const AbstractValue &LocalTransAdmin::portValue( const string name )
{
	MASSERTMSG( this->portTable->find( name ) != this->portTable->end(), "The requested IN Port (" + name + ") does not exist for some cell that uses the function '" +  functionName() + "'" );
	return *(*portTable)[ name ];
}
