#ifndef __TUPLE_NODE_H__
#define __TUPLE_NODE_H__

#include <vector>

#include "synnode.h"
#include "tuple_value.h"


typedef std::vector<SyntaxNode*> v_tuple;


class TupleType : public TypeValue
{
public:
	TupleType();
	static const TupleType TheTuple;
};


class TupleNode : public SyntaxNode
{
private:
	Tuple<SyntaxNode*> tuple;

public:
	TupleNode(v_tuple*);
	TupleNode(const Tuple<SyntaxNode*>&);

	SyntaxNode *clone();

	const std::string name();

	value_ptr evaluate();

	const TypeValue &type() const;

	bool checkType() const;

	std::ostream &print(std::ostream&);
};


class TuplePositionNode : public SyntaxNode
{
private:
	SyntaxNode *tuple;
	SyntaxNode *pos;

public:
	TuplePositionNode(SyntaxNode*, SyntaxNode*);

	SyntaxNode *clone();

	const std::string name();

	value_ptr evaluate();

	const TypeValue &type() const;

	bool checkType() const;

	std::ostream &print(std::ostream&);
};

#endif
