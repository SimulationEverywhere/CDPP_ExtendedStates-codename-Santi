/*******************************************************************
*
*  DESCRIPTION: Real Functions
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 11/4/1999 (v2)
*
*******************************************************************/

#ifndef __REAL_FUNC_H
#define __REAL_FUNC_H

/** include files **/
#include <functional>
#include "real.h" 

using namespace std;
/** declarations **/

value_ptr valueWithQuantum(const AbstractValue &r, const Real& q);	// Devuelve el nuevo valor de r
					// segun el quantum q

Real atanh( const Real &r );		// Devuelve la tangente hiperbolica inversa de r
Real atan( const Real &r );		// Devuelve el arco tangente de r
Real asinh( const Real &r );		// Devuelve el seno hiperbolico inverso de r
Real asin( const Real &r );		// Devuelve el arco seno de r
Real acosh( const Real &r );		// Devuelve el coseno hiperbolico inverso de r
Real acos( const Real &r );		// Devuelve el arco coseno de r
Real cos( const Real &r );		// Devuelve el coseno de r
Real cosh( const Real &r );		// Devuelve el coseno hiperbolico de r
Real trunc( const Real &r );		// Trunca el valor de r
Real truncUpper( const Real &r );	// Trunca hacia arriba el valor de r
Real log( const Real &r );		// Devuelve el log (base 10) de r
Real ln( const Real &r );		// Devuelve el log natural de r
Real exp( const Real &r );		// Devuelve e ^ r
Real abs( const Real &r );		// Devuelve el valor absoluto de r
Real remainder( const Real &r1, const Real &r2 ); // Calcula el resto de r1/r2
Real power( const Real &r1, const Real &r2 ); // Calcula r1^r2
Real fractional( const Real &r );	// Devuelve la parte decimal de r
Real round( const Real &r );		// Redondea r al entero mas cercano
Real sin( const Real &r );		// Calcula el seno de r
Real sec( const Real &r );		// Calcula la secante de r
Real asec( const Real &r );		// Calcula el arco secante de r
Real acotan( const Real &r );		// Calcula el arco cotangente de r
Real asech( const Real &r );		// Calcula el arco secante hiperbolico de r
Real acosech( const Real &r );		// Calcula el arco cosecante hiperbolico de r
Real acotanh( const Real &r );		// Calcula el arco cotangente hiperbolico de r
Real sech( const Real &r );		// Calcula la secante hiperbolica de r
Real sinh( const Real &r );		// Calcula el seno hiperbolico de r
Real sqrt( const Real &r );		// Calcula la raiz cuadrada de r
Real tan( const Real &r );		// Calcula la tangente de r
Real cotan( const Real &r );		// Calcula la cotangente de r
Real cosec( const Real &r );		// Calcula la cosecante de r
Real cosech( const Real &r );		// Calcula la cosecante hiperbolica de r
Real tanh( const Real &r );		// Calcula la tangente hiperbolica de r
Real max( const Real &r1, const Real &r2 ); // Calcula el maximo entre r1 y r2
Real min( const Real &r1, const Real &r2 ); // Calcula el minimo entre r1 y r2
Real sign( const Real &r );		// Devuelve el signo de r
Real fact( const Real &r );		// Devuelve el factorial de r
Real logn( const Real &v, const Real &n ); // Calcula el logaritmo n-esimo de v
Real root( const Real &v, const Real &n ); // Calcula la raiz n-esima de v
Real comb( const Real &m, const Real &k ); // Calcula el combinatorio (m k)
Real mcm( const Real &r1, const Real &r2 ); // Calcula el minimo comun multiplo entre r1 y r2
Real gcd( const Real &r1, const Real &r2 ); // Calcula el maximo comun divisor entre r1 y r2
Real hip( const Real &c1, const Real &c2 ); // Calcula la hipotenusa del triangulo cuyos catetos son c1 y c2
Real rectToPolar_r( const Real &x, const Real &y ); // Convierte a coordenadas polares
Real rectToPolar_angle( const Real &x, const Real &y ); // Convierte a coordenadas polares
Real polarToRect_x( const Real &r, const Real &angle ); // Convierte a coordenadas cartesianas
Real polarToRect_y( const Real &r, const Real &angle ); // Convierte a coordenadas cartesianas
Real nextPrime( const Real &r );	// Devuelve el siguiente primo mayor a n
Real radToDeg( const Real &r );		// Convierte de Radianes a Grados
Real degToRad( const Real &r );		// Convierte de Grados a Radianes
Real nth_Prime( const Real &r );	// Devuelve el n-esimo primo, considerando el primer primo al numero 2.
Real CtoK( const Real &r );		// Convierte de Centigrados a Kelvin
Real CtoF( const Real &r );		// Convierte de Centigrados a Farenheit
Real FtoC( const Real &r );		// Convierte de Farenheit a Centrigrados
Real FtoK( const Real &r );		// Convierte de Farenheit a Kelvin
Real KtoC( const Real &r );		// Convierte de Kelvin a Centrigrados
Real KtoF( const Real &r );		// Convierte de Kelvin a Farenheit


Real random( const Real &r );		// Devuelve un numero aleatorio x (con distribucion uniforme) tal que 0 <= x <= 1
					// El parametro r no tiene mucho uso. Si r es indefinido devuelve indefinido, sino devuelve un numero aleatorio
Real randomSign( const Real &r );	// Devuelve un signo aleatorio (con distribucion uniforme). El valor devuelto es 1 o -1 (con probabilidad 0.5 c/u)
					// El parametro r no tiene mucho uso. Si r es indefinido devuelve indefinido, sino devuelve un signo aleatorio
Real beta( const Real &a, const Real &b); // Devuelve un numero aleatorio x (con distribucion beta)
Real chi( const Real &r );		// Devuelve un numero aleatorio x (con distribucion chi cuadrado, con r grados de libertad)
Real randInt( const Real &r );		// Devuelve un numero aleatorio entero x tal que x pertenezca a [0,n] (con uniforme)
Real f( const Real &a, const Real &b);  // Devuelve un numero aleatorio x (con distribucion F), con a y b los grados de libertad del numerador y denominador respectivamente
Real exponential( const Real &r );	// Devuelve un numero aleatorio x (con distribucion Exponencial, con media r)
Real gamma( const Real &a, const Real &b); // Devuelve un numero aleatorio x (con distribucion Gamma)
Real normal( const Real &a, const Real &b); // Devuelve un numero aleatorio x (con distribucion Normal(a,b) )
Real uniform( const Real &a, const Real &b); // Devuelve un numero aleatorio x entre a y b (con distribucion uniforme)
Real binomial( const Real &n, const Real &p); // Devuelve un numero aleatorio x (con distribucion binomial, con n el numero de intentos y p la probabilidad de exito )
Real poisson( const Real &m );		// Devuelve un numero aleatorio x (con distribucion Poisson, con media m)

TValBool even( const Real &r );		// Dice si el numero es entero y par
TValBool odd( const Real &r );		// Dice si el numero es entero e impar
TValBool isInt( const Real &r );	// Dice si el numero es entero
TValBool isPrime( const Real &r );	// Dice si el numero es entero y primo
TValBool isUndefinedReal( const Real &r ); // Dice si el numero es indefinido

Real realIf( const TValBool &c, const Real &t, const Real &f);
					// Si c es verdadero devuelve t, sino devuelve f.
Real realIfu( const TValBool &c, const Real &t, const Real &f, const Real &u);
					// Si c es verdadero devuelve t, si es falso devuelve f, si es indefinido devuelve u.


template <class T, class Z>
struct r_tan : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (tan) ";

		return tan(t);
	}

	std::string type(){ return "TAN";}
};

template <class T, class Z>
struct r_tanh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (tanh) ";

		return tanh(t);
	}

	std::string type(){ return "TANH";}
};

template <class T, class Z>
struct r_sqrt : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sqrt) ";

		return sqrt(t);
	}

	std::string type(){ return "SQRT";}
};

template <class T, class Z>
struct r_sinh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sinh) ";

		return sinh(t);
	}

	std::string type(){ return "SINH";}
};


template <class T, class Z>
struct r_sin : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sin) ";

		return sin(t);
	}

	std::string type(){ return "SIN";}
};

template <class T, class Z>
struct r_round : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (round) ";

		return round(t);
	}

	std::string type(){ return "ROUND";}
};

template <class T, class Z>
struct r_fractional : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (fractional) ";

		return fractional(t);
	}

	std::string type(){ return "FRACTIONAL";}
};

template <class T, class Z>
struct r_remainder : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (remainder) ";

		return remainder(t1,t2);
	}

	std::string type(){ return "REMAINDER";}
};

template <class T, class Z>
struct r_power : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (power) ";

		return power(t1,t2);
	}

	std::string type(){ return "POWER";}
};

template <class T, class Z>
struct r_max : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (max) ";

		return max(t1,t2);
	}

	std::string type(){ return "MAX";}
};

template <class T, class Z>
struct r_min : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (min) ";

		return min(t1,t2);
	}

	std::string type(){ return "MIN";}
};

template <class T, class Z>
struct r_logn : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (logn) ";

		return logn(t1,t2);
	}

	std::string type(){ return "LOGN";}
};

template <class T, class Z>
struct r_root : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (root) ";

		return root(t1,t2);
	}

	std::string type(){ return "ROOT";}
};

template <class T, class Z>
struct r_comb : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (comb) ";

		return comb(t1,t2);
	}

	std::string type(){ return "COMB";}
};

template <class T, class Z>
struct r_abs : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (abs) ";

		return abs(t);
	}

	std::string type(){ return "ABS";}
};

template <class T, class Z>
struct r_exp : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (exp) ";

		return exp(t);
	}

	std::string type(){ return "EXP";}
};

template <class T, class Z>
struct r_ln : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (ln) ";

		return ln(t);
	}

	std::string type(){ return "LN";}
};

template <class T, class Z>
struct r_log : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (log) ";

		return log(t);
	}

	std::string type(){ return "LOG";}
};

template <class T, class Z>
struct r_cosh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (cosh) ";

		return cosh(t);
	}

	std::string type(){ return "COSH";}
};

template <class T, class Z>
struct r_cos : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (cos) ";

		return cos(t);
	}

	std::string type(){ return "COS";}
};

template <class T, class Z>
struct r_sec : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sec) ";

		return sec(t);
	}

	std::string type(){ return "SEC";}
};

template <class T, class Z>
struct r_sech : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sech) ";

		return sech(t);
	}

	std::string type(){ return "SECH";}
};

template <class T, class Z>
struct r_trunc : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (trunc) ";

		return trunc(t);
	}

	std::string type(){ return "TRUNC";}
};

template <class T, class Z>
struct r_truncUpper : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (truncUpper) ";

		return truncUpper(t);
	}

	std::string type(){ return "TRUNCUPPER";}
};

template <class T, class Z>
struct r_atanh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (atanh) ";

		return atanh(t);
	}

	std::string type(){ return "ATANH";}
};

template <class T, class Z>
struct r_atan : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (atan) ";

		return atan(t);
	}

	std::string type(){ return "ATAN";}
};

template <class T, class Z>
struct r_asinh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (asinh) ";

		return asinh(t);
	}

	std::string type(){ return "ASINH";}
};

template <class T, class Z>
struct r_asin : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (asin) ";

		return asin(t);
	}

	std::string type(){ return "ASIN";}
};

template <class T, class Z>
struct r_acosh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (acosh) ";

		return acosh(t);
	}

	std::string type(){ return "ACOSH";}
};

template <class T, class Z>
struct r_acos : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (acos) ";

		return acos(t);
	}

	std::string type(){ return "ACOS";}
};

template <class T, class Z>
struct r_cotan : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (cotan) ";

		return cotan(t);
	}

	std::string type(){ return "COTAN";}
};

template <class T, class Z>
struct r_asec : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (asec) ";

		return asec(t);
	}

	std::string type(){ return "ASEC";}
};

template <class T, class Z>
struct r_acotan : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (acotan) ";

		return acotan(t);
	}

	std::string type(){ return "ACOTAN";}
};

template <class T, class Z>
struct r_asech : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (asech) ";

		return asech(t);
	}

	std::string type(){ return "ASECH";}
};

template <class T, class Z>
struct r_acosech : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (acosech) ";

		return acosech(t);
	}

	std::string type(){ return "ACOSECH";}
};

template <class T, class Z>
struct r_acotanh : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (acotanh) ";

		return acotanh(t);
	}

	std::string type(){ return "ACOTANH";}
};

template <class T, class Z>
struct r_CtoF : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (CtoF) ";

		return CtoF(t);
	}

	std::string type(){ return "C_F";}
};

template <class T, class Z>
struct r_CtoK : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (CtoK) ";

		return CtoK(t);
	}

	std::string type(){ return "C_K";}
};

template <class T, class Z>
struct r_FtoC : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (FtoC) ";

		return FtoC(t);
	}

	std::string type(){ return "F_C";}
};

template <class T, class Z>
struct r_FtoK : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (FtoK) ";

		return FtoK(t);
	}

	std::string type(){ return "F_K";}
};

template <class T, class Z>
struct r_KtoC : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (KtoC) ";

		return KtoC(t);
	}

	std::string type(){ return "K_C";}
};

template <class T, class Z>
struct r_KtoF : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (KtoF) ";

		return KtoF(t);
	}
	std::string type(){ return "K_F";}
};


template <class T, class Z>
struct r_cosec : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (cosec) ";

		return cosec(t);
	}

	std::string type(){ return "COSEC";}
};

template <class T, class Z>
struct r_cosech : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (cosech) ";

		return cosech(t);
	}

	std::string type(){ return "COSECH";}
};

template <class T, class Z>
struct r_sign : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (sign) ";

		return sign(t);
	}

	std::string type(){ return "SIGN";}
};

template <class T, class Z>
struct r_random : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (random) ";

		return random(t);
	}

	std::string type(){ return "RANDOM";}
};

template <class T, class Z>
struct r_even : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (even) ";

		return even(t);
	}

	std::string type(){ return "EVEN";}
};

template <class T, class Z>
struct r_odd : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (odd) ";

		return odd(t);
	}

	std::string type(){ return "ODD";}
};

template <class T, class Z>
struct r_isInt : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (isInt) ";

		return isInt(t);
	}

	std::string type(){ return "ISINT";}
};

template <class T, class Z>
struct r_isPrime : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (isPrime) ";

		return isPrime(t);
	}

	std::string type(){ return "ISPRIME";}
};

template <class T, class Z>
struct r_isUndef : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (isUndefined) ";

		return isUndefinedReal(t);
	}

	std::string type(){ return "ISUNDEF";}
};

template <class T, class Z>
struct r_fact : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (fact) ";

		return fact(t);
	}

	std::string type(){ return "FACT";}
};

template <class T, class Z>
struct r_chi : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (chi) ";

		return chi(t);
	}

	std::string type(){ return "CHI";}
};

template <class T, class Z>
struct r_randInt : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (randInt) ";

		return randInt(t);
	}

	std::string type(){ return "RANDINT";}
};

template <class T, class Z>
struct r_beta : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (beta) ";

		return beta(t1,t2);
	}

	std::string type(){ return "BETA";}
};

template <class T, class Z>
struct r_exponential : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (exponential) ";

		return exponential(t);
	}

	std::string type(){ return "EXPONENTIAL";}
};

template <class T, class Z>
struct r_f : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (f) ";

		return f(t1,t2);
	}

	std::string type(){ return "F";}
};

template <class T, class Z>
struct r_gamma : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (gamma) ";

		return gamma(t1,t2);
	}

	std::string type(){ return "GAMMA";}
};

template <class T, class Z>
struct r_normal : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (normal) ";

		return normal(t1,t2);
	}

	std::string type(){ return "NORMAL";}
};

template <class T, class Z>
struct r_uniform : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (uniform) ";

		return uniform(t1,t2);
	}

	std::string type(){ return "UNIFORM";}
};

template <class T, class Z>
struct r_binomial : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (binomial) ";

		return binomial(t1,t2);
	}

	std::string type(){return "BINOMIAL";}
};

template <class T, class Z>
struct r_poisson : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (poisson) ";

		return poisson(t);
	}

	std::string type(){ return "POISSON";}
};

template <class T, class Z>
struct r_mcm : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (lcm) ";

		return mcm(t1,t2);
	}

	std::string type(){ return "MCM";}
};

template <class T, class Z>
struct r_gcd : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (gcd) ";

		return gcd(t1,t2);
	}

	std::string type(){ return "GCD";}
};

template <class T, class Z>
struct r_hip : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (hip) ";

		return hip(t1,t2);
	}

	std::string type(){ return "HIP";}
};


template <class T, class Z>
struct r_RectToPolar_r : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (RectToPolar_r) ";

		return rectToPolar_r(t1,t2);
	}

	std::string type(){ return "RECT_POLARR";}
};


template <class T, class Z>
struct r_RectToPolar_angle : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (RectToPolar_angle) ";

		return rectToPolar_angle(t1,t2);
	}

	std::string type(){ return "RECT_POLARA";}
};


template <class T, class Z>
struct r_PolarToRect_x : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (PolarToRect_x) ";

		return polarToRect_x(t1,t2);
	}

	std::string type(){ return "POLAR_RECTX";}
};


template <class T, class Z>
struct r_PolarToRect_y : public binary_function< T, T, Z>
{
	Z operator()(const T& t1, const T& t2) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (PolarToRect_y) ";

		return polarToRect_y(t1,t2);
	}

	std::string type(){ return "POLAR_RECTY";}
};

template <class T, class Z>
struct r_nextPrime : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (nextPrime) ";

		return nextPrime(t);
	}

	std::string type(){ return "NEXTPRIME";}
};

template <class T, class Z>
struct r_radToDeg : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (radToDeg) ";

		return radToDeg(t);
	}

	std::string type(){ return "RAD_DEG";}
};

template <class T, class Z>
struct r_degToRad : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (degTorad) ";

		return degToRad(t);
	}

	std::string type(){ return "DEG_RAD";}
};

template <class T, class Z>
struct r_nth_Prime : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (Nth_Prime) ";

		return nth_Prime(t);
	}

	std::string type(){ return "NTH_PRIME";}
};

template <class T, class Z>
struct r_randomSign : public unary_function< T, Z>
{
	Z operator()(const T& t) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (randomSign) ";

		return randomSign(t);
	}

	std::string type(){ return "RANDOM_SIGN";}
};

/****************************************************************************/
template <class Arg1, class Arg2, class Arg3, class Result>
struct threeChild_function {
	typedef Arg1 first_argument_type;
	typedef Arg2 second_argument_type;
	typedef Arg3 third_argument_type;
	typedef Result result_type;
};

template <class Arg1, class Arg2, class Arg3, class Arg4, class Result>
struct fourChild_function {
	typedef Arg1 first_argument_type;
	typedef Arg2 second_argument_type;
	typedef Arg3 third_argument_type;
	typedef Arg4 fourth_argument_type;
	typedef Result result_type;
};
/****************************************************************************/


template <class T1, class T2, class Z>
struct r_if : public threeChild_function< T1, T2, T2, Z>
{
	Z operator()(const T1& t1, const T2& t2, const T2& t3) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (if) ";

		return realIf(t1,t2,t3);
	}

	std::string type(){ return "IF";}
};

template <class T1, class T2, class Z>
struct r_ifu : public fourChild_function< T1, T2, T2, T2, Z>
{
	Z operator()(const T1& t1, const T2& t2, const T2& t3, const T2& t4) const
	{
		if (EvalDebug().Active())
			EvalDebug().Stream() << " (ifu) ";

		return realIfu(t1,t2,t3,t4);
	}

	std::string type(){ return "IFU";}
};







typedef r_tan< Real, Real >                 REAL_Tan;
typedef r_tanh< Real, Real >                REAL_Tanh;
typedef r_sqrt< Real, Real >                REAL_Sqrt;
typedef r_sinh< Real, Real >                REAL_Sinh;
typedef r_sin< Real, Real >                 REAL_Sin;
typedef r_round< Real, Real >               REAL_Round;
typedef r_fractional< Real, Real >          REAL_Fractional;
typedef r_remainder< Real, Real >           REAL_Remainder;
typedef r_abs< Real, Real >                 REAL_Abs;
typedef r_exp< Real, Real >                 REAL_Exp;
typedef r_ln< Real, Real >                  REAL_Ln;
typedef r_log< Real, Real >                 REAL_Log;
typedef r_power< Real, Real >               REAL_Power;
typedef r_cosh< Real, Real >                REAL_Cosh;
typedef r_cos< Real, Real >                 REAL_Cos;
typedef r_sec< Real, Real >                 REAL_Sec;
typedef r_sech< Real, Real >                REAL_Sech;
typedef r_trunc< Real, Real >               REAL_Trunc;
typedef r_truncUpper< Real, Real >          REAL_TruncUpper;
typedef r_atanh< Real, Real >               REAL_Atanh;
typedef r_asinh< Real, Real >               REAL_Asinh;
typedef r_asin< Real, Real >                REAL_Asin;
typedef r_atan< Real, Real >                REAL_Atan;
typedef r_acosh< Real, Real >               REAL_Acosh;
typedef r_acos< Real, Real >                REAL_Acos;
typedef r_max< Real, Real >                 REAL_Max;
typedef r_min< Real, Real >                 REAL_Min;
typedef r_sign< Real, Real >                REAL_Sign;
typedef r_logn< Real, Real >                REAL_Logn;
typedef r_root< Real, Real >                REAL_Root;
typedef r_fact< Real, Real >                REAL_Fact;
typedef r_comb< Real, Real >                REAL_Comb;
typedef r_mcm< Real, Real >                 REAL_MCM;
typedef r_gcd< Real, Real >                 REAL_GCD;
typedef r_hip< Real, Real >                 REAL_HIP;
typedef r_RectToPolar_r< Real, Real >       REAL_RECTTOPOLAR_R;
typedef r_RectToPolar_angle< Real, Real >   REAL_RECTTOPOLAR_ANGLE;
typedef r_PolarToRect_x< Real, Real >       REAL_POLARTORECT_X;
typedef r_PolarToRect_y< Real, Real >       REAL_POLARTORECT_Y;
typedef r_cotan< Real, Real >               REAL_Cotan;
typedef r_cosec< Real, Real >               REAL_Cosec;
typedef r_cosech< Real, Real >              REAL_Cosech;
typedef r_nextPrime< Real, Real >           REAL_NextPrime;
typedef r_radToDeg< Real, Real >            REAL_RadToDeg;
typedef r_degToRad< Real, Real >            REAL_DegToRad;
typedef r_nth_Prime< Real, Real >           REAL_Nth_Prime;
typedef r_asec< Real, Real >                REAL_Asec;
typedef r_acotan< Real, Real >              REAL_Acotan;
typedef r_asech< Real, Real >               REAL_Asech;
typedef r_acosech< Real, Real >             REAL_Acosech;
typedef r_acotanh< Real, Real >             REAL_Acotanh;
typedef r_CtoF< Real, Real >                REAL_CtoF;
typedef r_CtoK< Real, Real >                REAL_CtoK;
typedef r_KtoC< Real, Real >                REAL_KtoC;
typedef r_KtoF< Real, Real >                REAL_KtoF;
typedef r_FtoC< Real, Real >                REAL_FtoC;
typedef r_FtoK< Real, Real >                REAL_FtoK;

typedef r_random< Real, Real >              REAL_Random;
typedef r_randomSign< Real, Real >          REAL_RandomSign;
typedef r_beta< Real, Real >                REAL_Beta;
typedef r_chi< Real, Real >                 REAL_Chi;
typedef r_randInt< Real, Real >             REAL_RandInt;
typedef r_exponential< Real, Real >         REAL_Exponential;
typedef r_f< Real, Real >                   REAL_F;
typedef r_gamma< Real, Real >               REAL_GAMMA;
typedef r_normal< Real, Real >              REAL_NORMAL;
typedef r_uniform< Real, Real >             REAL_UNIFORM;
typedef r_binomial< Real, Real >            REAL_Binomial;
typedef r_poisson< Real, Real >             REAL_Poisson;

typedef r_even< Real, TValBool >            REAL_Even;
typedef r_odd< Real, TValBool >             REAL_Odd;
typedef r_isPrime< Real, TValBool >         REAL_IsPrime;
typedef r_isInt< Real, TValBool >           REAL_IsInt;
typedef r_isUndef< Real, TValBool >         REAL_IsUndefined;

typedef r_if< TValBool, Real, Real >        REAL_IF;
typedef r_ifu< TValBool, Real, Real >       REAL_IFU;

#endif	// REAL_FUNC_H
