#ifndef __TUPLE_VALUE_H__
#define __TUPLE_VALUE_H__

#include <sstream>
#include <ostream>
#include <vector>

#include "impresion.h"
#include "tbool.h"
#include "value.h"


template<class T> class Tuple;


template<class T>
using tuple_ptr = std::shared_ptr<Tuple<T> >;


template<class T>
class Tuple : public AbstractValue
{
public:
	Tuple() {};
	Tuple(std::vector<T> *v) : values(*v) {};
	Tuple(const std::vector<T> *v) : values(*v) {};
	Tuple(const std::initializer_list<T> &l) : values(l) {};

	const T &operator[](int) const;
	int size() const;

	CellValueType type() const;

	void print(std::ostream&) const;

	std::string asString(int width = Impresion::Default.Width(), int precision = Impresion::Default.Precision()) const;

	bool equals(const AbstractValue&) const;

	static tuple_ptr<T> ptr_from_value(const AbstractValue&);
	static tuple_ptr<T> ptr_from_value(const value_ptr&);
	static Tuple<T> from_value(const AbstractValue&);
	static Tuple<T> from_value(const value_ptr&);

	template<class S>
	friend std::istream &operator>>(std::istream&, Tuple<S>&);

protected:
	virtual value_ptr clone() const;

private:
	std::vector<T> values;
};


template<class T>
const T &Tuple<T>::operator[](int index) const
{
	MASSERT(index >= 0 && index < this->values.size());

	return this->values[index];
}

template<class T>
int Tuple<T>::size() const
{
	return this->values.size();
}

template<class T>
value_ptr Tuple<T>::clone() const
{
	value_ptr ptr(new Tuple<T>(&this->values));
	return ptr;
}

template<class T>
CellValueType Tuple<T>::type() const
{
	return CellValueType::tuple;
}

template<class T>
void Tuple<T>::print(std::ostream &os) const
{
	os << "[";
	std::string sep;

	for(auto value: this->values)
	{
		os << sep << value;
		sep = ", ";
	}

	os << "]";
}

template<class T>
std::string Tuple<T>::asString(int width, int precision) const
{
	std::stringstream sstr;
	this->print(sstr);
	return sstr.str();
}

template<class T>
bool Tuple<T>::equals(const AbstractValue& val) const
{
	tuple_ptr<T> t_ptr = Tuple<T>::ptr_from_value(val);
	if(t_ptr == nullptr)
		return false;

	if(this->size() != t_ptr->size())
		return false;

	for(int i = 0; i < this->size(); ++i)
		if((*this)[i] != (*t_ptr)[i])
			return false;

	return true;
}

template<class T>
tuple_ptr<T> Tuple<T>::ptr_from_value(const AbstractValue& val)
{
	if(val.type() != CellValueType::tuple)
		return nullptr;

	const Tuple<T> *t = dynamic_cast<const Tuple<T>*>(&val);
	return tuple_ptr<T>(new Tuple<T>(*t));
}

template<class T>
tuple_ptr<T> Tuple<T>::ptr_from_value(const value_ptr& val)
{
	if(val->type() != CellValueType::tuple)
		return nullptr;

	const tuple_ptr<T> t = std::dynamic_pointer_cast<Tuple<T> >(val);
	return tuple_ptr<T>(new Tuple<T>(*t));
}

template<class T>
Tuple<T> Tuple<T>::from_value(const AbstractValue& val)
{
	return *Tuple<T>::ptr_from_value(val);
}

template<class T>
Tuple<T> Tuple<T>::from_value(const value_ptr& val)
{
	return *Tuple<T>::ptr_from_value(val);
}

template<class T>
std::istream &operator>>(std::istream& is, Tuple<T> &t)
{
	std::vector<T> v;
	T value;
	char delim;
	bool bad = false;

	is >> delim;

	if(delim != '[')
	{
		is.putback(delim);
		is.setstate(std::ios::failbit);
		bad = true;
	}
	else
	{
		while(is.peek() != ']')
		{
			if(!(is >> value))
			{
				is.setstate(std::ios::failbit);
				bad = true;
				break;
			}

			if(is.peek() != ',' &&
			   is.peek() != ']')
			{
				is.setstate(std::ios::failbit);
				bad = true;
				break;
			}

			v.push_back(value);

			if(is.peek() == ',')
				is.ignore();
		}
	}

	if(!bad)
		t.values = v;

	return is;
}

#endif
