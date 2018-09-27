/*******************************************************************
*
*  DESCRIPTION: This file contains all the classes to represent
*               a boolean expresion syntax tree and the classes
*               to manage type values. They are:
*                    class SyntaxNode (abstract base class)
*                    class SpecNode (the full sepecification as a rule set)
*                    class RuleNode (just one ryle)
*                    class CountNode (the function count node)
*                    class VarNode (variables)
*                    class PortRefNode (variables)
*                    class ConstantNode (constants)
*                    class TimeNode (variables)
*		     class SendPortNode (for send a value to a output port)
*                    class AbsCellPosNode (for cellPos function)
*                    class ListNode (lista de SyntaxNode*)
*                    class StringNode (constants) (only for the portNames)
*                    template class OpNode< ReturnType > (base operation)
*                    template class UnaryOpNode< Operation, ReturnType, ParameterType > (unary operation)
*                    template class BinaryOpNode< Operation, ReturnType, ParameterType > (binary operation)
*                    template class ThreeOpNode< Operation, ReturnType, ParameterType >
*                    template class FourOpNode< Operation, ReturnType, ParameterType >
*                    class TypeValue
*                    class IntType
*                    class BoolType
*                    class RealType
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 02/04/1999 (v2)
*
*******************************************************************/

#ifndef __SYNTAX_NODE_H
#define __SYNTAX_NODE_H

/** include files **/
#include <iostream>
#include <functional>
#include <list>
#include "tbool.h"
#include "real.h"
#include "realfunc.h"
#include "ltranadm.h"

/** foward declarations **/
class NeighborhoodValue;
class ListNode;

/** declarations **/

class TypeValue
{
public:
	virtual ~TypeValue()
	{}

	enum Type
	{
		tabstract,
		tint,
		tbool,
		treal,
		tportvalue,
		ttuple
	} ;

	TypeValue( Type ty = tabstract ): t( ty )
	{}

	Type type() const
	{return t ;}

	std::ostream &print( std::ostream &os )
	{return os << ( t == tint ? "int" : (t == tbool ? "bool" : ( t == treal ? "real" : "portvalue" ) ) ) ;}

	bool isValid( const TypeValue &ty ) const
	{return t == ty.type() ;}

protected:

private:
	Type t ;
} ; // TypeValue

class IntType: public TypeValue
{
public:
	IntType(): TypeValue( tint )
	{}
	static const IntType TheInt ;
} ; // IntType 


class BoolType: public TypeValue
{
public:
	BoolType(): TypeValue( tbool )
	{}
	static const BoolType TheBool ;
} ; // BoolType 


class RealType: public TypeValue
{
public:
	RealType(): TypeValue( treal )
	{}
	static const RealType TheReal ;
} ; // RealType 


class PortValueType: public TypeValue
{
public:
	PortValueType(): TypeValue( tportvalue )
	{}
	static const PortValueType ThePortValue ;
} ; // PortValueType 


inline
	std::ostream &operator <<( std::ostream &os, TypeValue &nval )
{
	return nval.print( os ) ;
}


class SyntaxNode
{
public:
	virtual ~SyntaxNode()
	{}

	virtual const std::string name() = 0 ;

	virtual SyntaxNode *clone() = 0 ;

	virtual value_ptr evaluate() = 0 ;

	virtual const TypeValue &type() const = 0 ;

	virtual bool checkType() const = 0 ;

	virtual std::ostream &print( std::ostream & ) = 0 ;

protected:
	SyntaxNode()
	{}	// default constructor

private:

}; // SyntaxNode


inline
	std::ostream &operator <<( std::ostream &os, SyntaxNode &node )
{
	return node.print( os ) ;
}

/*****************************************
* Class VarNode
* Representa la referencia a una celda.
*****************************************/
class VarNode: public SyntaxNode
{
public:
	VarNode( nTupla nt, std::string pName = "" ) : portname( pName )
	{ tupla = nt; }

	SyntaxNode *clone()
	{ return new VarNode( tupla, portname ); }

	const std::string name()
	{ return "VarRef"; }

	value_ptr evaluate() ;

	const TypeValue &type() const
	{ return RealType::TheReal; }

	bool checkType() const
	{ return true; }

	std::ostream &print( std::ostream &os );
	
	std::string &port(void)
	{ return portname; }
	
	VarNode &port(std::string pName)
	{ portname = pName; return *this; }

private:
	nTupla	tupla;
	std::string  portname;

} ; // VarRefNode


/*****************************************
* Class PortRefNode
* Representa la referencia a un port de entrada.
*****************************************/
class PortRefNode: public SyntaxNode
{
public:
	PortRefNode( SyntaxNode *x = NULL ): portName( x )
	{}

	~PortRefNode()
	{ delete portName; }

	SyntaxNode *clone()
	{return new PortRefNode( portName ) ;}

	const std::string name()
	{return "PortRef" ;}

	value_ptr evaluate() ;

	const TypeValue &type() const
	{return RealType::TheReal ;}

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os )
	{os << " (" << portName << ")" ; return os ;}

private:
	SyntaxNode	*portName ;

} ; // PortRefNode


/*****************************************
* Class SendPortNode
* Envia un valor a un puerto de salida de la celda
*****************************************/
class SendPortNode: public SyntaxNode
{
public:
	SendPortNode( SyntaxNode *x = NULL, SyntaxNode *y = NULL ): portName( x ), portValue( y )
	{}

	~SendPortNode()
	{ delete portName; delete portValue;}

	SyntaxNode *clone()
	{return new SendPortNode( portName, portValue ) ;}

	const std::string name()
	{return "SendPort" ;}

	value_ptr evaluate();

	const TypeValue &type() const
	{return RealType::TheReal;}

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os )
	{os << " (" << portName << ", " << portValue << ")"; return os;}

private:
	std::string PortName();

	SyntaxNode	*portName ;
	SyntaxNode	*portValue ;

} ; // SendPortNode


/*****************************************
* Class SendPortNode
* Envia un valor a un puerto de salida de la celda
*****************************************/
class SendNCPortNode: public SyntaxNode
{
public:
	SendNCPortNode( SyntaxNode *x = NULL, SyntaxNode *y = NULL ): portName( x ), portValue( y )
	{}

	~SendNCPortNode()
	{ delete portName; delete portValue;}

	SyntaxNode *clone()
	{return new SendNCPortNode( portName, portValue ) ;}

	const std::string name()
	{return "SendNCPort" ;}

	value_ptr evaluate();

	const TypeValue &type() const
	{return RealType::TheReal;}

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os )
	{os << " (" << portName << ", " << portValue << ")"; return os;}
	
	PortValue getPortValue();

private:
	std::string PortName();

	SyntaxNode *portName ;
	SyntaxNode *portValue ;
};


/***************************************
* class ConstantNode
* Representa constantes numericas y logicas del lenguaje
***************************************/
class ConstantNode: public SyntaxNode
{
public:
	ConstantNode( Real n, const TypeValue &t ) : value( n ), tval( t )
	{}
	~ConstantNode()
	{}

	const std::string name()
	{return "Constant" ;}

	value_ptr evaluate();

	SyntaxNode *clone()
	{return new ConstantNode( value, tval ) ;}

	const TypeValue &type() const
	{return tval ;}

	std::ostream &print( std::ostream &os )
	{return os << value ;}

	bool checkType() const
	{return true ;}

private:
	Real value ;
	const TypeValue &tval ;

};	// ContantNode 


/***************************************
* class StringNode
* Representa el nombre de un portIn
***************************************/
class StringNode: public SyntaxNode
{
public:
	StringNode( std::string n ) : value( n )
	{}

	~StringNode()
	{}

	const std::string name()
	{return "String" ;}

	value_ptr evaluate()
	{ return AbstractValue::to_value_ptr(Real(0)); }

	std::string getString();

	SyntaxNode *clone()
	{return new StringNode( value ) ;}

	std::ostream &print( std::ostream &os )
	{return os << value ;}

	const TypeValue &type() const
	{return RealType::TheReal ;}

	bool checkType() const
	{return true ;}

private:
	std::string value ;

};	// StringNode 



/***************************************
* class TimeNode
* Representa un nodo que contiene el tiempo de simulacion
***************************************/
class TimeNode: public SyntaxNode
{
public:
	TimeNode( )
	{}

	const std::string name()
	{return "TimeNode" ;}

	SyntaxNode *clone()
	{ return new TimeNode() ; }

	value_ptr evaluate()
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << "Evaluate: VTime of Simulation = " << SingleLocalTransAdmin::Instance().actualTime().asMsecs() << "\n";

		return AbstractValue::to_value_ptr(SingleLocalTransAdmin::Instance().actualTime().asMsecs());
	}

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os )
	{return os << "VTime " << SingleLocalTransAdmin::Instance().actualTime() ;}

	const TypeValue &type() const
	{return RealType::TheReal ;}

};	// TimeNode 


/***************************************
* class AbsCellPosNode
* Representa un nodo que contiene el la posicion de la celda que esta ejecutando la evaluacion
***************************************/
class AbsCellPosNode: public SyntaxNode
{
public:
	AbsCellPosNode(SyntaxNode *x)
		: posIndex(x)
	{}

	~AbsCellPosNode()
	{ delete posIndex; }

	const std::string name()
	{return "AbsCellPosNode";}

	SyntaxNode *clone()
	{ return new AbsCellPosNode(posIndex); }

	value_ptr evaluate();

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os );

	const TypeValue &type() const
	{return RealType::TheReal ;}

private:
	SyntaxNode	*posIndex;
};	// TimeNode 


/***************************************
* class StateVarNode
* Representa una referencia a una variable de estado de la celda
***************************************/
class StateVarNode: public SyntaxNode
{
public:
	StateVarNode( const std::string &VarName ) : varName(VarName)
	{}
	~StateVarNode()
	{}

	const std::string name()
	{ return "StateVar"; }

	value_ptr evaluate();

	SyntaxNode *clone()
	{ return new StateVarNode( varName ); }

	const TypeValue &type() const
	{ return RealType::TheReal; }

	std::ostream &print( std::ostream &os )
	{ return os << varName; }

	bool checkType() const
	{ return true ; }
	
	const std::string &getVarName()
	{ return varName; }

private:
	std::string varName;

};	// StateVarNode 



/***************************************
* class AssignNode
* Representa una asignación a una variable de estado
***************************************/
class AssignNode: public SyntaxNode
{
public:
	AssignNode(SyntaxNode *var = NULL, SyntaxNode *val = NULL)
	{ variable = var; value = val; }

	~AssignNode()
	{ if (variable != NULL) delete variable;
	  if (value != NULL) delete value; }

	const std::string name()
	{ return "Assign"; }

	value_ptr evaluate();

	SyntaxNode *clone()
	{ return new AssignNode(variable, value); }
	
	void var(SyntaxNode *v)
	{ variable = v; }

	void val(SyntaxNode *v)
	{ value = v; }

	const TypeValue &type() const
	{ return RealType::TheReal; }

	std::ostream &print( std::ostream &os )
	{ variable->print(os); os << " := "; value->print(os); return os; }

	bool checkType() const
	{ return variable->type().isValid(value->type()); }

private:
	SyntaxNode *variable;
	SyntaxNode *value;

};	// AssignNode 


/***************************************
* class ListNode
* Representa una std::lista de SysntaxNodes
***************************************/
class ListNode: public SyntaxNode
{
public:
	ListNode() {}

	~ListNode();	

	const std::string name()
	{ return "ListNode"; }

	value_ptr evaluate();
	
	std::list<PortValue> getPortValues();

	SyntaxNode *clone()
	{ return new ListNode(NodeList); }
	
	void add(SyntaxNode *sn)
	{ NodeList.push_front(sn); }

	const TypeValue &type() const
	{ return RealType::TheReal; }

	std::ostream &print(std::ostream &os);

	bool checkType() const;

private:
	typedef std::list<SyntaxNode *> SynNodeList;
	
	ListNode(SynNodeList &snl)
	   : NodeList(snl)
	{}
	
	SynNodeList NodeList;
	
};	// ListNode 


/*****************************************
* class RuleNode
* Representa una regla de la especificacion. Dentro de sus componentes se
* encuentran la expresion que corresponde a la demora, el valor que debe
* retornar la regla, las asignaciones y la expresion de logica trivalente
* asociada.
******************************************/
class RuleNode: public SyntaxNode
{
public:
	RuleNode( ListNode *v = NULL,
	          ListNode *a = NULL,
		  SyntaxNode *d = NULL,
		  SyntaxNode *be = NULL ): val( v ), asgn( a ), dly( d ), boolExp( be )
	{}

	~RuleNode()
	{ delete val; delete dly; delete boolExp;
	  if (asgn != NULL) delete asgn; }

	const std::string name()
	{return "RuleNode" ;}

	SyntaxNode *clone()
	{return new RuleNode() ;}

	value_ptr evaluate();

	const TypeValue &type() const
	{return RealType::TheReal;}

	bool checkType() const;

	std::ostream &print( std::ostream &os );

	std::list<PortValue> value()
	{ return this->val->getPortValues(); }

	value_ptr assign()
	{ if (asgn == NULL) return AbstractValue::to_value_ptr(Real::tundef);
	  else              return this->asgn->evaluate(); }

	Real delay()
	{ return Real::from_value(this->dly->evaluate()); }

private:
	ListNode *val, *asgn;
	SyntaxNode *dly, *boolExp;

} ; // RuleNode 



template <class Value>
class OpNode: public SyntaxNode
{
public:
	OpNode()
	{}

	const std::string name()
	{return "Operation" ;}

	const TypeValue &type() const
	{return this->val ;}

protected:
	Value val ;
} ; // OpNode


template < class Operation, class ReturnType, class ParameterType >
class UnaryOpNode: public OpNode<ReturnType>
{
public:
	UnaryOpNode( SyntaxNode *n = NULL ): node( n )
	{}

	~UnaryOpNode()
	{delete node ;}

	UnaryOpNode &child( SyntaxNode *n )
	{node = n ; return *this ;} 

	value_ptr evaluate()
	{ 
		Real child = Real::from_value(this->child()->evaluate());
		Real aux = op(child);

		if (EvalDebug().Active())
		{
			EvalDebug().Stream() << "Evaluate: " << this->name() << "(" << child << ") = ";
			EvalDebug().Stream() << aux << "\n";
		}
		return AbstractValue::to_value_ptr(aux);
	}

	const std::string name()
	{return "UnaryOp" ;}

	SyntaxNode *child()
	{return node ;}

	SyntaxNode *clone()
	{return new UnaryOpNode< Operation, ReturnType, ParameterType >( node ) ;}

	std::ostream &print( std::ostream &os )
	{this->child()->print( os ) ; return os ;} 

	bool checkType() const
	{ParameterType a ; return this->node->checkType() && this->node->type().isValid( a ) ;}

private:
	SyntaxNode *node ;
	Operation op ;

} ; // UnaryOpNode


template <class Operation, class ReturnType, class ParameterType >
class BinaryOpNode: public OpNode<ReturnType>
{
public:
	BinaryOpNode( SyntaxNode *left = NULL , SyntaxNode *right = NULL ): l( left ), r( right )
	{}

	virtual ~BinaryOpNode()
	{delete l ; delete r ;}

	BinaryOpNode &left( SyntaxNode *n )
	{l = n ; return *this ;}

	BinaryOpNode &right( SyntaxNode *n )
	{r = n ; return *this ;}

	virtual value_ptr evaluate()
	{
		Real left = Real::from_value(this->left()->evaluate());
		Real right = Real::from_value(this->right()->evaluate());
		Real aux = op(left, right);

		if (EvalDebug().Active())
		{
			EvalDebug().Stream() << "Evaluate: " << this->name() << "(" << left << ", " << right << ") = ";
			EvalDebug().Stream() << aux << "\n";
		}

		if ( op.type() == T_AND_TYPE && TValBool::tfalse.eqv(left))
			return AbstractValue::to_value_ptr(0);
		else if ( op.type() == T_OR_TYPE && TValBool::ttrue.eqv(left))
			return AbstractValue::to_value_ptr(1);
		else if ( op.type() == T_XOR_TYPE && TValBool::tundef.eqv(left))
			return AbstractValue::to_value_ptr(UNDEFINED);
		else if ( op.type() == T_IMP_TYPE && TValBool::tfalse.eqv(left))
			return AbstractValue::to_value_ptr(1);

		return AbstractValue::to_value_ptr(aux);
	}

	const std::string name()
	{return "BinaryOp";}

	SyntaxNode *left()
	{return l;}

	SyntaxNode *right()
	{return r;}

	virtual SyntaxNode *clone()
	{return new BinaryOpNode<Operation, ReturnType, ParameterType>( l, r );}

	std::ostream &print( std::ostream &os )
	{this->left()->print( os ) << " BOP "; this->right()->print(os); return os ;}

	bool checkType() const
	{return this->l->checkType() && this->r->checkType() && this->l->type().isValid(RealType::TheReal) && this->r->type().isValid(RealType::TheReal);}

protected:
	SyntaxNode *l, *r ;
private:
	Operation op ;

} ; // BinaryOpNode


template <class Operation, class ReturnType, class ParameterType >
class BinaryAbsOpNode: public BinaryOpNode<Operation, ReturnType, ParameterType >
{
public:
	BinaryAbsOpNode( SyntaxNode *left = NULL , SyntaxNode *right = NULL ): BinaryOpNode<Operation, ReturnType, ParameterType>(left,right)
	{}

	value_ptr evaluate()
	{
		value_ptr left = this->left()->evaluate();
		value_ptr right = this->right()->evaluate();
		Real aux = op(*left, *right);

		if (EvalDebug().Active())
		{
			EvalDebug().Stream() << "Evaluate: " << this->name() << "(" << left << ", " << right << ") = ";
			EvalDebug().Stream() << aux << "\n";
		}

		if ( op.type() == T_AND_TYPE && TValBool::tfalse.eqv(Real::from_value(left)))
			return AbstractValue::to_value_ptr(0);
		else if ( op.type() == T_OR_TYPE && TValBool::ttrue.eqv(Real::from_value(left)))
			return AbstractValue::to_value_ptr(1);
		else if ( op.type() == T_XOR_TYPE && TValBool::tundef.eqv(Real::from_value(left)))
			return AbstractValue::to_value_ptr(UNDEFINED);
		else if ( op.type() == T_IMP_TYPE && TValBool::tfalse.eqv(Real::from_value(left)))
			return AbstractValue::to_value_ptr(1);

		return AbstractValue::to_value_ptr(aux);
	}

	SyntaxNode *clone()
	{
		SyntaxNode *_l = BinaryOpNode<Operation, ReturnType, ParameterType>::l;
		SyntaxNode *_r = BinaryOpNode<Operation, ReturnType, ParameterType>::r;
		return new BinaryAbsOpNode<Operation, ReturnType, ParameterType>( _l, _r );
	}

private:
	Operation op;
};


template <class Operation, class ReturnType, class ParameterType >
class ThreeOpNode: public OpNode<ReturnType>
{
public:
	ThreeOpNode( SyntaxNode *child1 = NULL , SyntaxNode *child2 = NULL, SyntaxNode *child3 = NULL ): c1( child1 ), c2( child2 ), c3( child3 )
	{}

	~ThreeOpNode()
	{delete c1 ; delete c2 ; delete c3; }

	ThreeOpNode &child1( SyntaxNode *n )
	{c1 = n ; return *this ;}

	ThreeOpNode &child2( SyntaxNode *n )
	{c2 = n ; return *this ;}

	ThreeOpNode &child3( SyntaxNode *n )
	{c3 = n ; return *this ;}

	value_ptr evaluate()
	{
		Real child1 = Real::from_value( this->child1()->evaluate() );
		Real child2 = Real::from_value( this->child2()->evaluate() );
		Real child3 = Real::from_value( this->child3()->evaluate() );
		Real aux = op(child1, child2, child3);

		if (EvalDebug().Active())
		{

			EvalDebug().Stream() << "Evaluate: " << this->name() << "(" << child1 << ", " << child2 << ", " << child3 << ") = ";
			EvalDebug().Stream() << aux << "\n";
		}

		return AbstractValue::to_value_ptr(aux);
	}

	const std::string name()
	{return "ThreeOp" ;}

	SyntaxNode *child1()
	{return c1 ;}

	SyntaxNode *child2()
	{return c2 ;}

	SyntaxNode *child3()
	{return c3 ;}

	SyntaxNode *clone()
	{return new ThreeOpNode<Operation, ReturnType, ParameterType>( c1, c2, c3 ) ;}

	std::ostream &print( std::ostream &os )
	{this->child1()->print( os ) << " ThreeOP " ;
		this->child2()->print( os ) ;
		this->child3()->print( os ) ;
		return os ;}

	bool checkType() const
	{ParameterType a ; return this->c1->checkType() && this->c2->checkType() && this->c3->checkType() && this->c1->type().isValid( a ) && this->c2->type().isValid( a ) && this->c3->type().isValid( a ) ;}

private:
	SyntaxNode *c1, *c2, *c3 ;
	Operation op ;

} ; // ThreeOpNode



template <class Operation, class ReturnType, class ParameterType >
class FourOpNode: public OpNode<ReturnType>
{
public:
	FourOpNode( SyntaxNode *child1 = NULL , SyntaxNode *child2 = NULL, SyntaxNode *child3 = NULL, SyntaxNode *child4 = NULL ): c1( child1 ), c2( child2 ), c3( child3 ), c4( child4 )
	{}

	~FourOpNode()
	{delete c1 ; delete c2 ; delete c3; delete c4; }

	FourOpNode &child1( SyntaxNode *n )
	{c1 = n ; return *this ;}

	FourOpNode &child2( SyntaxNode *n )
	{c2 = n ; return *this ;}

	FourOpNode &child3( SyntaxNode *n )
	{c3 = n ; return *this ;}

	FourOpNode &child4( SyntaxNode *n )
	{c4 = n ; return *this ;}

	value_ptr evaluate()
	{
		Real child1 = Real::from_value( this->child1()->evaluate() );
		Real child2 = Real::from_value( this->child2()->evaluate() );
		Real child3 = Real::from_value( this->child3()->evaluate() );
		Real child4 = Real::from_value( this->child4()->evaluate() );
		Real aux = op(child1, child2, child3, child4);

		if (EvalDebug().Active())
		{
			EvalDebug().Stream() << "Evaluate: " << this->name() << "(" << child1 << ", " << child2 << ", " << child3 << ", " << child4 << ") = ";
			EvalDebug().Stream() << aux << "\n";
		}

		return AbstractValue::to_value_ptr(aux);
	}

	const std::string name()
	{return "FourOp" ;}

	SyntaxNode *child1()
	{return c1 ;}

	SyntaxNode *child2()
	{return c2 ;}

	SyntaxNode *child3()
	{return c3 ;}

	SyntaxNode *child4()
	{return c4 ;}

	SyntaxNode *clone()
	{return new FourOpNode<Operation, ReturnType, ParameterType>( c1, c2, c3, c4 ) ;}

	std::ostream &print( std::ostream &os )
	{this->child1()->print( os ) << " ThreeOP " ;
		this->child2()->print( os ) ;
		this->child3()->print( os ) ;
		this->child4()->print( os ) ;
		return os ;}

	bool checkType() const
	{ParameterType a ; return this->c1->checkType() && this->c2->checkType() && this->c3->checkType() && this->c4->checkType() && this->c1->type().isValid( a ) && this->c2->type().isValid( a ) && this->c3->type().isValid( a ) && this->c4->type().isValid( a ) ;}

private:
	SyntaxNode *c1, *c2, *c3, *c4 ;
	Operation op ;

} ; // FourOpNode


/********************************************
* class SpecNode
* Represent a std::list of rules.
********************************************/
class SpecNode: public SyntaxNode
{
public:
	SpecNode()
	{} 

	~SpecNode();

	const std::string name()
	{return "SpecNode" ;}

	SyntaxNode *clone()
	{return new SpecNode() ;}

	const TypeValue &type() const
	{return RealType::TheReal ;}

	value_ptr evaluate() ;

	Real evaluate(bool anyUndefined, bool anyStochast)
	{
		anyUndef = anyUndefined;
		anyStochastic = anyStochast;
		return Real::from_value(evaluate());
	}

	bool checkType() const ;

	std::ostream &print( std::ostream &os ) ;

	SpecNode &addRule( SyntaxNode *rule, int StochasticCondition ) ;

	// ** Queries ** //
	std::list<PortValue> value() const
	{return lastValue ;}

	Real delay() const
	{return lastDelay ;}

	const std::string elseFunction() const
	{ return ElseFunction ; }

	void elseFunction( const std::string ef )
	{ ElseFunction = ef; }

	const bool AnyUndef() const
	{ return anyUndef; }

	const bool AnyStochastic() const
	{ return anyStochastic; }

	void resetWarningCondition()
	{ anyUndef = anyStochastic = false; }

private:
	typedef struct {
		SyntaxNode	*Node;
		bool		StochasticNode;
	} Rule;
	
	typedef std::list< Rule > RuleList;
	RuleList rules;
	std::string ElseFunction ;
	Real lastDelay; 
	std::list<PortValue> lastValue;
	bool anyUndef, anyStochastic;

} ; // SpecNode 


class InvalidEvaluation: public MException
{
public:
	InvalidEvaluation( const std::string &text ): MException( text )
	{}

	InvalidEvaluation &addNeighborhood( const NeighborhoodValue & ) ;
};


class CountNode: public SyntaxNode
{
public:
	CountNode( const Real &v, StringNode *p = NULL): value( v )
	{ evalType = valReal;
	  if (p == NULL)
	  	portName = new StringNode("");
	  else
  	  	portName = p; }

	CountNode( SyntaxNode * &s, StringNode *p = NULL): sn( s )
	{ evalType = toEval;
	  if (p == NULL)
	  	portName = new StringNode("");
	  else
  	  	portName = p; }

	~CountNode()
	{ delete sn; delete portName; }

	const std::string name()
	{return "CountNode" ;}

	SyntaxNode *clone()
	{return new CountNode( value, portName ) ;}

	value_ptr evaluate() ;

	bool checkType() const
	{return true ;}

	std::ostream &print( std::ostream &os )
	{ return os << "Count " << value.value() << " on " << portName->getString(); }

	const TypeValue &type() const
	{return IntType::TheInt ;}

protected:
	typedef enum { valReal, toEval } typeToEval;

private:
	typeToEval evalType;			// Say if 'value' contains
						// a real or an expresion to
						// be evaluated.
	Real value ;
	SyntaxNode *sn;
	StringNode *portName;
	

} ; // CountNode


////////////////////////////////////////////////////////////////////////////
// operators
////////////////////////////////////////////////////////////////////////////
typedef UnaryOpNode< TVB_not, BoolType, BoolType > NOTNode ;
typedef BinaryOpNode< TVB_and, BoolType, BoolType > ANDNode ;
typedef BinaryOpNode< TVB_or, BoolType, BoolType > ORNode ;
typedef BinaryOpNode< TVB_Xor, BoolType, BoolType > XORNode ;
typedef BinaryOpNode< TVB_imp, BoolType, BoolType > IMPNode ;
typedef BinaryOpNode< TVB_eqv, BoolType, BoolType > EQVNode ;

typedef UnaryOpNode< REAL_Even, BoolType, RealType > FuncEVEN ;
typedef UnaryOpNode< REAL_Odd, BoolType, RealType > FuncODD ;
typedef UnaryOpNode< REAL_IsInt, BoolType, RealType > FuncISINT ;
typedef UnaryOpNode< REAL_IsPrime, BoolType, RealType > FuncISPRIME ;
typedef UnaryOpNode< REAL_IsUndefined, BoolType, RealType > FuncISUNDEFINED ;

typedef BinaryOpNode< REAL_Plus, RealType, RealType > PlusNode ;
typedef BinaryOpNode< REAL_Minus, RealType, RealType > MinusNode ;
typedef BinaryOpNode< REAL_Divides, RealType, RealType > DividesNode ;
typedef BinaryOpNode< REAL_Multiplies, RealType, RealType > MultipliesNode ;

typedef BinaryAbsOpNode< Abstract_Equal_to, BoolType, TypeValue > EqualNode ;
typedef BinaryAbsOpNode< Abstract_Not_equal_to, BoolType, TypeValue > NotEqualNode ;

typedef BinaryOpNode< REAL_Less, BoolType, RealType > LessNode ;
typedef BinaryOpNode< REAL_Greater, BoolType, RealType > GreaterNode ;
typedef BinaryOpNode< REAL_Less_equal, BoolType, RealType > LessEqualNode ;
typedef BinaryOpNode< REAL_Greater_equal, BoolType, RealType > GreaterEqualNode ;

typedef UnaryOpNode< REAL_Tan, RealType, RealType > FuncTAN ;
typedef UnaryOpNode< REAL_Tanh, RealType, RealType > FuncTANH ;
typedef UnaryOpNode< REAL_Sqrt, RealType, RealType > FuncSQRT ;
typedef UnaryOpNode< REAL_Sinh, RealType, RealType > FuncSINH ;
typedef UnaryOpNode< REAL_Sin, RealType, RealType > FuncSIN ;
typedef UnaryOpNode< REAL_Round, RealType, RealType > FuncROUND ;
typedef UnaryOpNode< REAL_Fractional, RealType, RealType > FuncFRACTIONAL ;
typedef BinaryOpNode< REAL_Remainder, RealType, RealType > FuncREMAINDER ;
typedef BinaryOpNode< REAL_Power, RealType, RealType > FuncPOWER ;
typedef UnaryOpNode< REAL_Abs, RealType, RealType > FuncABS ;
typedef UnaryOpNode< REAL_Exp, RealType, RealType > FuncEXP ;
typedef UnaryOpNode< REAL_Ln, RealType, RealType > FuncLN ;
typedef UnaryOpNode< REAL_Log, RealType, RealType > FuncLOG ;
typedef UnaryOpNode< REAL_Cosh, RealType, RealType > FuncCOSH ;
typedef UnaryOpNode< REAL_Cos, RealType, RealType > FuncCOS ;
typedef UnaryOpNode< REAL_Sec, RealType, RealType > FuncSEC ;
typedef UnaryOpNode< REAL_Sech, RealType, RealType > FuncSECH ;
typedef UnaryOpNode< REAL_Trunc, RealType, RealType > FuncTRUNC ;
typedef UnaryOpNode< REAL_TruncUpper, RealType, RealType > FuncTRUNCUPPER ;
typedef UnaryOpNode< REAL_Atanh, RealType, RealType > FuncATANH ;
typedef UnaryOpNode< REAL_Atan, RealType, RealType > FuncATAN ;
typedef UnaryOpNode< REAL_Asinh, RealType, RealType > FuncASINH ;
typedef UnaryOpNode< REAL_Asin, RealType, RealType > FuncASIN ;
typedef UnaryOpNode< REAL_Acosh, RealType, RealType > FuncACOSH ;
typedef UnaryOpNode< REAL_Acos, RealType, RealType > FuncACOS ;
typedef BinaryOpNode< REAL_Max, RealType, RealType > FuncMAX ;
typedef BinaryOpNode< REAL_Min, RealType, RealType > FuncMIN ;
typedef UnaryOpNode< REAL_Sign, RealType, RealType > FuncSIGN ;
typedef UnaryOpNode< REAL_Fact, RealType, RealType > FuncFACT ;
typedef BinaryOpNode< REAL_Logn, RealType, RealType > FuncLOGN ;
typedef BinaryOpNode< REAL_Root, RealType, RealType > FuncROOT ;
typedef BinaryOpNode< REAL_Comb, RealType, RealType > FuncCOMB ;
typedef UnaryOpNode< REAL_Random, RealType, RealType > FuncRANDOM ;
typedef BinaryOpNode< REAL_Beta, RealType, RealType > FuncBETA ;
typedef UnaryOpNode< REAL_Chi, RealType, RealType > FuncCHI ;
typedef UnaryOpNode< REAL_RandInt, RealType, RealType > FuncRANDINT ;
typedef BinaryOpNode< REAL_F, RealType, RealType > FuncF ;
typedef UnaryOpNode< REAL_Exponential, RealType, RealType > FuncEXPONENTIAL ;
typedef BinaryOpNode< REAL_GAMMA, RealType, RealType > FuncGAMMA ;
typedef BinaryOpNode< REAL_NORMAL, RealType, RealType > FuncNORMAL ;
typedef BinaryOpNode< REAL_UNIFORM, RealType, RealType > FuncUNIFORM ;
typedef BinaryOpNode< REAL_Binomial, RealType, RealType > FuncBINOMIAL ;
typedef UnaryOpNode< REAL_Poisson, RealType, RealType > FuncPOISSON ;
typedef BinaryOpNode< REAL_MCM, RealType, RealType > FuncMCM ;
typedef BinaryOpNode< REAL_GCD, RealType, RealType > FuncGCD ;
typedef BinaryOpNode< REAL_HIP, RealType, RealType > FuncHIP ;
typedef BinaryOpNode< REAL_RECTTOPOLAR_R, RealType, RealType > FuncRECTTOPOLAR_R ;
typedef BinaryOpNode< REAL_RECTTOPOLAR_ANGLE, RealType, RealType > FuncRECTTOPOLAR_ANGLE ;
typedef BinaryOpNode< REAL_POLARTORECT_X, RealType, RealType > FuncPOLARTORECT_X ;
typedef BinaryOpNode< REAL_POLARTORECT_Y, RealType, RealType > FuncPOLARTORECT_Y ;
typedef UnaryOpNode< REAL_Cotan, RealType, RealType > FuncCOTAN ;
typedef UnaryOpNode< REAL_Cosec, RealType, RealType > FuncCOSEC ;
typedef UnaryOpNode< REAL_Cosech, RealType, RealType > FuncCOSECH ;
typedef UnaryOpNode< REAL_Asec, RealType, RealType > FuncASEC ;
typedef UnaryOpNode< REAL_Acotan, RealType, RealType > FuncACOTAN ;
typedef UnaryOpNode< REAL_Asech, RealType, RealType > FuncASECH ;
typedef UnaryOpNode< REAL_Acosech, RealType, RealType > FuncACOSECH ;
typedef UnaryOpNode< REAL_Acotanh, RealType, RealType > FuncACOTANH ;
typedef UnaryOpNode< REAL_CtoF, RealType, RealType > FuncCTOF ;
typedef UnaryOpNode< REAL_CtoK, RealType, RealType > FuncCTOK ;
typedef UnaryOpNode< REAL_FtoC, RealType, RealType > FuncFTOC ;
typedef UnaryOpNode< REAL_FtoK, RealType, RealType > FuncFTOK ;
typedef UnaryOpNode< REAL_KtoF, RealType, RealType > FuncKTOF ;
typedef UnaryOpNode< REAL_KtoC, RealType, RealType > FuncKTOC ;

typedef UnaryOpNode< REAL_NextPrime, RealType, RealType > FuncNEXTPRIME ;
typedef UnaryOpNode< REAL_RadToDeg, RealType, RealType > FuncRADTODEG ;
typedef UnaryOpNode< REAL_DegToRad, RealType, RealType > FuncDEGTORAD ;
typedef UnaryOpNode< REAL_Nth_Prime, RealType, RealType > FuncNTH_PRIME ;
typedef UnaryOpNode< REAL_RandomSign, RealType, RealType > FuncRANDOMSIGN ;

typedef ThreeOpNode< REAL_IF, RealType, RealType > IFNode ;
typedef FourOpNode< REAL_IFU, RealType, RealType > IFUNode ;

/////////////////////////////////////////////////////////////////////////////
/** inline **/
/////////////////////////////////////////////////////////////////////////////

inline
	value_ptr ConstantNode::evaluate()
{
	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: Constant = " << value << "\n";

	return AbstractValue::to_value_ptr(value);
}

inline
	std::string StringNode::getString()
{
	if (EvalDebug().Active())
		EvalDebug().Stream() << "Evaluate: String (PortName) = " << value << "\n";

	return value ;
}

#endif // __SYNTAX_NODE_H
