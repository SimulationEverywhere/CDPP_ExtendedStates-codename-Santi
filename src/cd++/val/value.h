/*******************************************************************
*
*  DESCRIPTION: Value type definition
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 02/03/1999 (v2)
*
*******************************************************************/

#ifndef _VALUE_HPP
#define _VALUE_HPP

/** include files **/
#include <ostream>
#include <memory>

#include "impresion.h"


class Real;
class AbstractValue;
class TValBool;


/** definitions **/
typedef double Value;
typedef std::shared_ptr<AbstractValue> value_ptr;
typedef std::shared_ptr<Real> real_ptr;


enum CellValueType
{
	real,
	tuple
};


class AbstractValue
{
public:
	virtual ~AbstractValue() {};

	virtual CellValueType type() const = 0;

	virtual void print(std::ostream&) const = 0;

	virtual std::string asString(int width = Impresion::Default.Width(), int precision = Impresion::Default.Precision()) const = 0;

	virtual bool equals(const AbstractValue&) const = 0;

	friend TValBool operator==(const AbstractValue &, const AbstractValue &);
	friend TValBool operator!=(const AbstractValue &, const AbstractValue &);
	friend std::ostream &operator<<(std::ostream&, const AbstractValue&);

	static value_ptr from_string(const std::string&);
	static value_ptr to_value_ptr(const AbstractValue&);
	static value_ptr to_value_ptr(const double&);

protected:
	virtual value_ptr clone() const = 0;
};

#endif   //__VALUE_H
