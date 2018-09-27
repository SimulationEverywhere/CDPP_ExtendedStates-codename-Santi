/*******************************************************************
*
*  DESCRIPTION: class RealPrecision
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 2/03/1999 (v2)
*
*******************************************************************/

#ifndef __REALPREC_H
#define __REALPREC_H

/** include files **/
#include "value.h"

/** declarations **/
class RealPrecision
{
public:
	RealPrecision(Value Valor);	// Constructor
	RealPrecision();		// Constructor

	const Value precision() const;	// Obtiene el valor almacenado.
	const Value precision(Value Valor);
					// Establece el valor a usar como TOL

	static class RealPrecision Tol;

private:
	Value rvalue;			// Contiene el valor.

}; // RealPrecision



/** inline **/
inline
RealPrecision::RealPrecision()
{
	rvalue = 1e-8;
}

inline
RealPrecision::RealPrecision(Value valor)
{
	rvalue = valor;
}

inline
const Value RealPrecision::precision() const
{
	return rvalue;
}

inline
const Value RealPrecision::precision(Value Valor)
{
	return (rvalue = Valor);
}

#endif	// REALPREC_H
