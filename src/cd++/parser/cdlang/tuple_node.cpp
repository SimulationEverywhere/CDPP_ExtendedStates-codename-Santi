#include <string>
#include <vector>

#include "tuple_node.h"


const TupleType TupleType::TheTuple;


TupleType::TupleType() : TypeValue(ttuple)
{
}

TupleNode::TupleNode(v_tuple* t) : tuple(t)
{
}

TupleNode::TupleNode(const Tuple<SyntaxNode*> &t) : tuple(t)
{
}

SyntaxNode *TupleNode::clone()
{
	return new TupleNode(this->tuple);
}

const std::string TupleNode::name()
{
	return "TupleNode";
}

value_ptr TupleNode::evaluate()
{
	std::vector<Real> v;

	for(int i = 0; i < this->tuple.size(); ++i)
	{
		SyntaxNode *node = this->tuple[i];
		value_ptr node_val = node->evaluate();
		real_ptr r = std::dynamic_pointer_cast<Real>(node_val);
		v.push_back(*r);
	}

	value_ptr p(new Tuple<Real>(&v));

	return p;
}

const TypeValue &TupleNode::type() const
{
	return TupleType::TheTuple;
}

bool TupleNode::checkType() const
{
	for(int i = 0; i < this->tuple.size(); ++i)
	{
		if(!this->tuple[i]->checkType() ||
		   !this->tuple[i]->type().isValid(RealType::TheReal))
			return false;
	}

	return true;
}

std::ostream &TupleNode::print(std::ostream &os)
{
	// TODO
	return os;
}


TuplePositionNode::TuplePositionNode(SyntaxNode* t, SyntaxNode* c) : tuple(t), pos(c)
{
}

SyntaxNode *TuplePositionNode::clone()
{
	return new TuplePositionNode(this->tuple, this->pos);
}

const std::string TuplePositionNode::name()
{
	return "TuplePositionNode";
}

value_ptr TuplePositionNode::evaluate()
{
	value_ptr tup_ptr = this->tuple->evaluate();
	value_ptr pos_ptr = this->pos->evaluate();
	Real pos = Real::from_value(pos_ptr);

	tuple_ptr<Real> tup = Tuple<Real>::ptr_from_value(tup_ptr);

	// In case the left operand is not a tuple.
	if(tup == nullptr)
		return AbstractValue::to_value_ptr(Real::tundef);

	MASSERT(pos >= 0 && pos < tup->size());

	return AbstractValue::to_value_ptr((*tup)[pos.value()]);
}

const TypeValue &TuplePositionNode::type() const
{
	return RealType::TheReal;
}

bool TuplePositionNode::checkType() const
{
	return this->tuple->checkType() &&
		   this->tuple->type().isValid(TupleType::TheTuple) &&
		   this->pos->checkType() &&
		   this->pos->type().isValid(RealType::TheReal);
}

std::ostream &TuplePositionNode::print(std::ostream& os)
{
	// TODO
	return os;
}
