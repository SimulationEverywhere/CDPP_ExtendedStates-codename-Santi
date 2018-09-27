/*******************************************************************
*
*  DESCRIPTION: class Real
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 11/4/1999 (v2)
*
*******************************************************************/

#ifndef __REAL_H
#define __REAL_H

/** include files **/
#include <ostream>

#include "undefd.h" 
#include "impresion.h"
#include "value.h"
#include "tbool.h" 
#include "evaldeb.h"	// EvalDebug

/** forward declarations **/
class TValBool;

/** declarations **/
class Real : public AbstractValue
{
public:
	Real(Value Valor);		// Crea un real definido con
					// el valor especificado.

	Real() {rvalue = UNDEFINED;}	// Crea un real indefinido.
	
	const Value value() const {return rvalue;}
					// Obtiene el valor almacenado.

	const Value value(Value Valor) {return (rvalue=Valor);}
					// Establece el valor del Real

	void ValueUndef() {rvalue = UNDEFINED;}
					// Establece el valor del real como
					// indefinido

	bool IsDefined() const {return !_isUndefined(rvalue);}
					// Devuelve True si el real tiene
					// un valor definido. En otro caso
					// devuelve False.

	bool IsUndefined() const {return _isUndefined(rvalue);}
					// Devuelve True si el real tiene
					// un valor indefinido En otro caso
					// devuleve False.

	std::string asString(int width = Impresion::Default.Width(), int precision = Impresion::Default.Precision() ) const;
					// Devuelve el real como un std::string
					// pero con cierto formato.
	
	Real &operator = ( const Real & );

	TValBool operator!=(const double&) const;
	TValBool operator==(const double&) const;
	bool equals(const AbstractValue&) const;

	TValBool operator > ( const Real & ) const;
	TValBool operator < ( const Real & ) const;
	TValBool operator >= ( const Real & ) const;
	TValBool operator <= ( const Real & ) const;

	TValBool NotEqual ( const Real & ) const;
	TValBool Equal ( const Real & ) const;
	TValBool Greater ( const Real & ) const;
	TValBool Less ( const Real & ) const;
	TValBool GreaterEqual ( const Real & ) const;
	TValBool LessEqual ( const Real & ) const;

	const Real operator + (const Real &) const;
	const Real operator - (const Real &) const;
	const Real operator * (const Real &) const;
	const Real operator / (const Real &) const;
	
	operator TValBool() const;

	CellValueType type() const;


	void print(std::ostream&) const;

	static real_ptr ptr_from_value(const AbstractValue&);
	static real_ptr ptr_from_value(const value_ptr&);
	static Real from_value(const AbstractValue&);
	static Real from_value(const value_ptr&);

	friend std::istream &operator>>(std::istream&, Real&);

	static Real tundef;	// Constante
	static Real one;	// Constante
	static Real zero;	// Constante

protected:
	virtual value_ptr clone() const;

private:
	Value rvalue;			// Contiene el valor.

}; // Real



template <class T, class Z>
struct r_plus : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (+) ";

		return t1 + t2;
	}
	
	std::string type(){return "+";}
};

template <class T, class Z>
struct r_minus : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (-) ";

		return t1 - t2;
	} 
	
	std::string type(){return "-";}
};

template <class T, class Z>
struct r_divides : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (/) ";

		return t1 / t2;
	}
	
	std::string type(){return "/";}
};

template <class T, class Z>
struct r_multiplies : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (*) ";

		return t1 * t2;
	}
	
	std::string type(){return "*";}
};

template <class T, class Z>
struct r_equal_to : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (=) ";

		return t1 == t2; //t1.Equal(t2);
	}
	
	std::string type(){return "==";}
};

template <class T, class Z>
struct r_not_equal_to : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (!=) ";

		return t1 != t2; //t1.NotEqual(t2);
	}
	
	std::string type(){return "!=";}
};

template <class T, class Z>
struct r_less : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (<) ";

		return t1.Less(t2);
	}
	
	std::string type(){return "<";}
};

template <class T, class Z>
struct r_greater : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (>) ";

		return t1.Greater(t2);
	}
	
	std::string type(){return ">";}
};

template <class T, class Z>
struct r_greater_equal : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (>=) ";

		return t1.GreaterEqual(t2);
	}

	std::string type(){return ">=";}
};

template <class T, class Z>
struct r_less_equal : public std::binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
	 	if (EvalDebug().Active())
			EvalDebug().Stream() << " (<=) ";

		return t1.LessEqual(t2);
	}
	
	std::string type(){return "<=";}
};

typedef r_not_equal_to< AbstractValue, TValBool >    Abstract_Not_equal_to;
typedef r_equal_to< AbstractValue, TValBool >        Abstract_Equal_to;

typedef r_less_equal< Real, TValBool >      REAL_Less_equal;
typedef r_greater_equal< Real, TValBool >   REAL_Greater_equal;
typedef r_greater< Real, TValBool >         REAL_Greater;
typedef r_less< Real, TValBool >            REAL_Less;
typedef r_multiplies< Real, Real > 	    REAL_Multiplies;
typedef r_divides< Real, Real >             REAL_Divides;
typedef r_plus< Real, Real >                REAL_Plus;
typedef r_minus< Real, Real >               REAL_Minus;


/** inline **/
inline
std::ostream &operator << (std::ostream &os, Real r)
{
	if (r.IsUndefined())
		return os << "?";
	
	return os << r.value();
}

inline
Real &Real::operator = ( const Real &r )
{
	this->value( r.value() );
	return *this;
}

#endif	// Real_H

