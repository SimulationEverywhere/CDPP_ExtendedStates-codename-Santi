/*******************************************************************
*
*  DESCRIPTION: class SpecNode, VarNode and CountNode
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 13/06/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include <sstream>
#include <climits>

#include "synnode.h"
#include "neighval.h"         // NeighborhoodValue
#include "atomcell.h"
#include "strutil.h"          // lowerCase 

using namespace std;

// ** public data ** //
const RealType RealType::TheReal;
const IntType IntType::TheInt ;
const BoolType BoolType::TheBool ;


// ** public ** //
/*******************************************************************
* Method: destructor
********************************************************************/
SpecNode::~SpecNode()
{
	for (RuleList::iterator cursor = rules.begin(); cursor != rules.end(); cursor++)
		delete cursor->Node;
}
/*******************************************************************
* Method: addRule
********************************************************************/
SpecNode &SpecNode::addRule( SyntaxNode *rule, int StochasticCondition )
{
	Rule	r;
	r.Node = rule;
	r.StochasticNode = StochasticCondition != 0 ? true: false;

	rules.push_back( r ) ;
	return *this ;
}

/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr RuleNode::evaluate()
{
	value_ptr val = boolExp->evaluate();

	if (EvalDebug().Active())
	{
		Real aux = Real::from_value(val);
		EvalDebug().Stream() << "Evaluate: Rule  = " << (aux.IsUndefined() ? "Undefined" : (aux.value()==0 ? "False":"True")) << "\n";
		EvalDebug().Stream() << "\n";
	}

	return val;
}



/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr SpecNode::evaluate()
{
	register int	total = 1, validRules = 0;
	register bool	avise = false;
	Real	        cursorEval;
	int             current_rule = 0, previous_rule = 0;

	if ( DebugCellRules().Active() )
		total = rules.size();

	lastValue.clear();

	RuleList::iterator cursor ;
	for( cursor = rules.begin() ; cursor != rules.end() && validRules < total ; cursor++ )
	{
		current_rule++;
		cursorEval = Real::from_value(cursor->Node->evaluate());

		if (cursor->StochasticNode)
			anyStochastic = true;

		if( cursorEval.value() == 1 )     // 1 == TRUE
		{
			validRules++ ;

			if( validRules == 1 )
			{
				previous_rule = current_rule;
				
				lastDelay = static_cast<RuleNode*>(cursor->Node)->delay() ;
				            static_cast<RuleNode*>(cursor->Node)->assign() ;
				lastValue = static_cast<RuleNode*>(cursor->Node)->value() ; 
			} else
			{
				if( (bool)(lastValue != static_cast<RuleNode*>(cursor->Node)->value())  &&  !anyStochastic )
				{
					ostringstream errormsg;
					
					errormsg << "Rules " << previous_rule << " and " << current_rule << " evaluate to TRUE and their results are different!";
					InvalidEvaluation e( errormsg.str() ) ;
					MTHROW( e ) ;
				}

				if( (bool)(lastDelay != static_cast<RuleNode*>(cursor->Node)->delay()) && !anyStochastic )
				{
					ostringstream errormsg;
					
					errormsg << "Rules " << previous_rule << " and " << current_rule << " evaluate to TRUE and their delay are different!";
					InvalidEvaluation e( errormsg.str() ) ;
					MTHROW( e ) ;
				}

				if (anyStochastic && !avise)
				{
					avise = true;
					cout << "\nWARNING. In the stochastic model, two o more rules evaluate to TRUE.\n";
				}
			}
		}
		else if ( cursorEval.IsUndefined() )	// Value = ?
			anyUndef = true;

//		else if ( cursorEval.value() == 0 ) 	// 0 = FALSE
//				;
	}

	if ( validRules <= 0 && anyUndef )
	{
		// If the evaluation of any rule is undefined then:
		if (elseFunction() == "")
			cout << "Warning! - None of the rules evaluate to True, but any evaluate to undefined.\n";

		return Real::to_value_ptr(Real::tundef);
	}
	else if ( validRules <= 0 && !anyUndef )
	{
		// IF all the rules evaluate to FALSE and none of them
		// have a 'random' like function

		if (elseFunction() == "")
		{
			if (anyStochastic)
			{
				cout << "Warning! - None of the rules evaluate to True in the Stochastic Model.\n";
				return Real::to_value_ptr(Real::zero);
			}
			else
			{
				InvalidEvaluation e( "None of the rules evaluate to TRUE!" ) ;
				MTHROW( e ) ;
			}
		}
		else
		{
			return Real::to_value_ptr(Real::zero);
		}
	}
	// If any rule evaluate to TRUE then
	return Real::to_value_ptr(Real::one);
}

/*******************************************************************
* Method: checkType
********************************************************************/
bool SpecNode::checkType() const
{
	RuleList::const_iterator cursor ;
	for( cursor = rules.begin() ; 
	cursor != rules.end() && cursor->Node->checkType() && cursor->Node->type().isValid( BoolType::TheBool ) ;
	cursor++ ) ;

	return cursor == rules.end() ;
}

/*******************************************************************
* Method: print
********************************************************************/
ostream &SpecNode::print( ostream &os )
{
	RuleList::iterator cursor ;
	for( cursor = rules.begin() ; cursor != rules.end() ; (*cursor++).Node->print( os ) ) ;

	return os ;
}

/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr VarNode::evaluate()
{
	string name(lowerCase(portname.empty() ? AtomicCell::neighborChangePort : portname));
	const AbstractValue &valueToReturn = SingleLocalTransAdmin::Instance().cellValue( NeighborPosition( tupla ), name );
	
	if (EvalDebug().Active()) {
		EvalDebug().Stream() << "Evaluate: Cell Reference" << tupla;
		if (!portname.empty())
			EvalDebug().Stream() << "~" << portname;
		EvalDebug().Stream() << " = " << valueToReturn << "\n";
	}
	return AbstractValue::to_value_ptr(valueToReturn);
}

/*******************************************************************
* Method: print
********************************************************************/
ostream &VarNode::print( ostream &os )
{
	os << tupla;
	if (!portname.empty())
		os << "~" << portname;
	
	return os;
}


/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr PortRefNode::evaluate()
{
	string name ( static_cast<StringNode *>(portName)->getString() );

	if (name == "thisport" )
		name = SingleLocalTransAdmin::Instance().portSource();

	const AbstractValue &valueToReturn = SingleLocalTransAdmin::Instance().portValue(name);

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: PortIn Reference(" << name << ") = " << valueToReturn << "\n";

	return AbstractValue::to_value_ptr(valueToReturn);
}

/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr SendPortNode::evaluate()
{
	string	           name( PortName() );
	value_ptr	           valor( portValue->evaluate() );
	VTime	           actualTime( SingleLocalTransAdmin::Instance().actualTime() );
	const Port        *port = NULL;

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: SendToPort Reference(" << name << ", " << *valor << ") at time " << SingleLocalTransAdmin::Instance().actualTime() << "\n";

	const Model *am = SingleLocalTransAdmin::Instance().actualModel();
	
	VirtualPortList *vpl;

	vpl = const_cast<VirtualPortList *>(& (SingleLocalTransAdmin::Instance().outputPorts()));
	port = getPort(vpl, name);
	
	MASSERTMSG(port != NULL, "The output port " + name + " is not defined with the LINK statement.\n");
	
	((Model *) am)->sendOutput((const VTime &) actualTime,
	                           (const Port &) *port,
				   *valor);

	return Real::to_value_ptr(0);
}


/*******************************************************************
* Method: PortName
********************************************************************/
string SendPortNode::PortName()
{
	string name;
	
	if (portName == NULL)
		name = AtomicCell::outPort;
	else
		name = static_cast<StringNode *>(portName)->getString();

	return name;
}


/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr SendNCPortNode::evaluate()
{
	string	           name( lowerCase(PortName()) );
	value_ptr	           valor = portValue->evaluate();
	PortList::iterator cursor;

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: SendToNCPort Reference(" << name << ", " << *valor << ") at time " << SingleLocalTransAdmin::Instance().actualTime() << "\n";

	AtomicCell *am = (AtomicCell *) SingleLocalTransAdmin::Instance().actualModel();
	PortList &pl = am->outNCPortList();

	if (PortName() != AtomicCell::outPort)
		name = AtomicCell::NCOutPrefix + name;

	cursor = pl.begin();
	while (cursor != pl.end() && lowerCase(cursor->second->name()) != name)
		cursor++;

	MASSERTMSG(cursor != pl.end(), "The NC output port " + PortName() + " does not exist.\n");

	return valor;
}


/*******************************************************************
* Method: PortName
********************************************************************/
string SendNCPortNode::PortName()
{
	string name;
	
	if (portName == NULL)
		name = AtomicCell::outPort;
	else
		name = static_cast<StringNode *>(portName)->getString();

	return name;
}


/*******************************************************************
* Method: getPortValue
********************************************************************/
PortValue SendNCPortNode::getPortValue()
{
	string name = PortName();
	
	if (name != AtomicCell::outPort)
		name = AtomicCell::NCOutPrefix + name;

	return PortValue(name, this->evaluate());
}


/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr AbsCellPosNode::evaluate()
{
	Real aux = Real::from_value(posIndex->evaluate());
	Real val = SingleLocalTransAdmin::Instance().neighborhood().centralPosition().get( (int) aux.value() );

	MASSERTMSG( aux.value() < INT_MAX, "The CellPos function has a parameter greater than the maximum integer allowed" );

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: CellPosition(" << (int) aux.value() << ") = " << val << "\n";

	return Real::to_value_ptr(val);
}

/*******************************************************************
* Function Name: evaluate
********************************************************************/
value_ptr CountNode::evaluate()
{
	if (evalType == toEval)
		value = Real::from_value(sn->evaluate());

	register int total( 0 ) ;
	string name(lowerCase(portName->getString()));

	if (name.empty())
		name = lowerCase(AtomicCell::neighborChangePort);
	else
		if (name != lowerCase(AtomicCell::neighborChangePort) && name.find(AtomicCell::NCInPrefix) != 0)
			name = AtomicCell::NCInPrefix + name;


	NeighborhoodValue *neighbors = (NeighborhoodValue *) &( SingleLocalTransAdmin::Instance().neighborhood() ) ;
	NeighborhoodValue::NeighborList neighborValues = neighbors->neighborValues();

	NeighborhoodValue::NeighborList::const_iterator cursor;

	for ( cursor = neighborValues.begin(); cursor != neighborValues.end() ; cursor++)
	{
		const AbstractValue &cell_val = SingleLocalTransAdmin::Instance().cellValue( cursor->first, name );
		Real val;

		switch(cell_val.type())
		{
			case CellValueType::real:
			{
				const real_ptr val_ptr = Real::ptr_from_value(cell_val);
				val = *val_ptr;
				break;
			}
			// Backward compatibility: for tuples we check whether the first
			// element equals the given value.
			case CellValueType::tuple:
			{
				const tuple_ptr<Real> val_ptr = Tuple<Real>::ptr_from_value(cell_val);
				MASSERT(val_ptr->size() > 0);
				val = (*val_ptr)[0];
				break;
			}
			default:
			{
				MException e;
				MTHROW(e);
			}
		}

		if( val == value )
			total++ ;
	}

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: CountNode(" << value << ", \"" << portName->getString() << "\") = " << total << "\n";

	return Real::to_value_ptr(total);
}

/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr StateVarNode::evaluate()
{
	AtomicCell *Cell;
	Cell = (AtomicCell *) (SingleLocalTransAdmin::Instance().actualModel());
	AbstractValue &valueToReturn = Cell->variable(varName);

	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: StateVar " << varName << " = " << valueToReturn << "\n";

	return AbstractValue::to_value_ptr(valueToReturn);
}


/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr AssignNode::evaluate()
{
	if (variable == NULL) {
		InvalidEvaluation e("No state variable to assign to");
		MTHROW(e);
	}

	if (variable->name() != StateVarNode("").name()) {
		InvalidEvaluation e("Left value is not a state variable");
		MTHROW(e);
	}

	if (value == NULL) {
		InvalidEvaluation e("No value to assign");
		MTHROW(e);
	}
	
	AtomicCell *Cell = (AtomicCell *) (SingleLocalTransAdmin::Instance().actualModel());
	string varName(((StateVarNode *) variable)->getVarName());
	value_ptr valueToAssign = value->evaluate();
	
	if (EvalDebug().Active()) {
		EvalDebug().Stream() << "Evaluate: Assign ";
		variable->print(EvalDebug().Stream());
		EvalDebug().Stream() << " := " << Real::from_value(valueToAssign).value() << "\n";
	}
		
	Cell->variable(varName, *valueToAssign);
	
	return valueToAssign;
}


/*******************************************************************
* Method: destructor
********************************************************************/
ListNode::~ListNode()
{
	while (!NodeList.empty()) {
		delete *NodeList.begin();
		NodeList.erase(NodeList.begin());
	}
}

/*******************************************************************
* Method: print
********************************************************************/
ostream &ListNode::print(ostream &os)
{
	SynNodeList::iterator cursor;
	
	os << "ListNode {\n";
	for (cursor = NodeList.begin(); cursor != NodeList.end(); cursor++) {
		(*cursor)->print(os);
		os << ";\n";
	}
	os << "}\n";
	
	return os;
}


/*******************************************************************
* Method:checkType
********************************************************************/
bool ListNode::checkType() const
{
	SynNodeList::const_iterator cursor = NodeList.begin();
	
	while (cursor != NodeList.end() && (*cursor)->checkType())
		cursor++;

	return (cursor == NodeList.end());
}


/*******************************************************************
* Method: evaluate
********************************************************************/
value_ptr ListNode::evaluate()
{
	SynNodeList::iterator cursor;
	value_ptr RetVal;
	
	for (cursor = NodeList.begin(); cursor != NodeList.end(); cursor++)
		RetVal = (*cursor)->evaluate();

	return RetVal;
}

/*******************************************************************
* Method: getPortValues
********************************************************************/
list<PortValue> ListNode::getPortValues()
{
	SynNodeList::iterator cursor;
	list<PortValue>       pvl;
	SendNCPortNode        *spn;
	
	for (cursor = NodeList.begin(); cursor != NodeList.end(); cursor++) {
		spn = (SendNCPortNode *) (*cursor);
		pvl.push_back(spn->getPortValue());
	}
	if (EvalDebug().Active())
		EvalDebug().Stream() << "\n";

	return pvl;
}

/*******************************************************************
* Function Name: addNeighborhood
********************************************************************/
InvalidEvaluation &InvalidEvaluation::addNeighborhood( const NeighborhoodValue &neighbors )
{
	NeighborhoodValue::NeighborList neighborValues = neighbors.neighborValues();
	NeighborhoodValue::NeighborList::const_iterator cursor;
	cursor = neighborValues.begin();

	register int dimension = 0;
	bool hasPorts = false;

	if ( cursor != neighborValues.end() ) {
		dimension = cursor->first.dimension();
		hasPorts = (cursor->second.size() > 1);
	}

	MASSERTMSG( dimension >= 2, "The dimension must be >= 2");

	if (dimension == 2 && !hasPorts)
	{
		addText( "The state of the Neighbors is: " );
		
		// primero calculo el valor de 'dim'
		long	dimXMin = 9999999, dimXMax = 0;
		long	dimYMin = 9999999, dimYMax = 0;
		for (; cursor != neighborValues.end(); cursor++ )
		{
			// Veo para 2 dimensiones (height, width)
			if ( cursor->first.get(DIM_WIDTH) > dimXMax )
				dimXMax = cursor->first.get(DIM_WIDTH);

			if ( cursor->first.get(DIM_WIDTH) < dimXMin )
				dimXMin = cursor->first.get(DIM_WIDTH);

			if ( cursor->first.get(DIM_HEIGHT) > dimYMax )
				dimYMax = cursor->first.get(DIM_HEIGHT);

			if ( cursor->first.get(DIM_HEIGHT) < dimYMin )
				dimYMin = cursor->first.get(DIM_HEIGHT);
		}	

		string line("+") ;
		for( int i = dimXMin ; i<= dimXMax ; i++ )
			for (int w = 0; w <= Impresion::Default.Width() + 1; w++)
			line += "-" ;
		line += "+" ;
		addText( line ) ;

		for( int i = dimYMin ; i<= dimYMax ; i++ )
		{
			line = "|" ;
			for( int j = dimXMin; j<= dimXMax; j++ ){
				nTupla	nt;
				nt.add(i);
				nt.add(j);
				line += string( " " );

				if (neighbors.isValid(nt))
					/* We currently are showing only the values arrived through
					   the neighborChange port. Should we show all-ports values? */
					line += neighbors.get( NeighborPosition(nt), AtomicCell::neighborChangePort ).asString();
				else
					for (int w = 0; w < Impresion::Default.Width(); w++)
						line += " " ;

				line += " " ;
			}
			line += "|" ;
			addText( line ) ;
		}

		line = "+" ;
		for( int i = dimXMin; i<= dimXMax; i++ )
			for (int w = 0; w <= Impresion::Default.Width() + 1; w++)
			line += "-" ;
		line += "+" ;
		addText( line ) ;
	}
	else
		// Estoy en 3 o mas dimensiones
		for (cursor = neighborValues.begin(); cursor != neighborValues.end(); cursor++ ) {
			NeighborhoodValue::CellPorts::const_iterator cursor2;
			
			for (cursor2 = cursor->second.begin(); cursor2 != cursor->second.end(); cursor2++)
				addText( cursor->first.print() + " => " + cursor2->first + " = " + cursor2->second->asString() );
		}

	return *this ;
}

/*******************************************************************
* Function Name: print
********************************************************************/
ostream &AbsCellPosNode::print( ostream &os )
{
	return os << "CellPosition " << SingleLocalTransAdmin::Instance().neighborhood().centralPosition();
}



/*******************************************************************
* Function Name: checkType
********************************************************************/
bool RuleNode::checkType() const
{
	bool res = true;
	
	if (asgn != NULL)
		res = this->asgn->checkType() &&
		      this->asgn->type().isValid( RealType::TheReal );
			
	return (this->boolExp->checkType()                         && 
	        this->boolExp->type().isValid( BoolType::TheBool ) && 
	        this->val    ->type().isValid( RealType::TheReal ) &&
	        this->dly    ->type().isValid( IntType::TheInt )   &&
	        res);
}


/*******************************************************************
* Function Name: print
********************************************************************/
ostream &RuleNode::print( ostream &os )
{
	string txt;
	
	os << *val << "/ ";
	if (asgn != NULL)
		os << *asgn;
		
	return  os << "/" << *dly << "/" << *boolExp;
}
