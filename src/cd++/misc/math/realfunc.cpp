/*******************************************************************
*
*  DESCRIPTION: Real Functions
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 2/3/1999 (v2)
*
*******************************************************************/

/** include files **/
#include <cstdlib>
#include "realfunc.h"
#include "tuple_value.h"
#include "mathincl.h"
extern "C"
{
#include "randlib.h"
}

using namespace std;

// Tolerance for the tangent
#define	TOL_TAN		7.55e-8

/* This function is only for Windows because it's not have the "rint" */
#ifdef __WINDOWS__
double rint(double x)
{
	double a1, fracc = modf( x, &a1 );
	return fracc < 0.5 ? floor(x) : ceil(x);
}
#endif


Real tan (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	Real	w( remainder( r, Real(2 * M_PI) ) );	// Normalizo

	// Veo si se producira overflow
	if ( fabs(w.value() - M_PI / 2) <= TOL_TAN   ||   fabs(w.value() - M_PI * 3 / 2) <= TOL_TAN)
		return Real( M_INFINITO );

	return Real( tan( w.value() ) );
}

Real tanh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( tanh( r.value() ) );
}

Real sec (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	Real w( cos( r ) );
	if (w.value() == 0)
		return Real( M_INFINITO );

	return Real( 1 / w.value() );
}

Real sech (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( 1 / cosh(r).value() );
}

Real sqrt (const Real &r)
{
	if ( r.IsUndefined() || r.value() < 0 )
		return Real();	// Devuelve un indefinido si es indefinido o negativo

	return Real( sqrt( r.value() ) );
}


Real sinh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( sinh( r.value() ) );
}

Real sin (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( sin( r.value() ) );
}

Real cos (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( cos( r.value() ) );
}

Real cosh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( cosh( r.value() ) );
}

Real trunc (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( floor( r.value() ) );
}

Real truncUpper (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( ceil( r.value() ) );
}

Real round (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( rint( r.value() ) );
}

Real fractional (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	double a1;
	return Real( modf( r.value(), &a1 ) );
}

Real remainder (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() || r2.value() == 0)
		return Real();	// Devuelve un indefinido

	return Real( fmod( r1.value(), r2.value() ) );
}

value_ptr valueWithQuantum(const AbstractValue &v, const Real& q)
{
	if (q.value() == 0)
		return AbstractValue::to_value_ptr(v);

	double ss;

	switch(v.type())
	{
		case CellValueType::real:
		{
			real_ptr r = Real::ptr_from_value(v);
			modf(r->value()/q.value() + DBL_EPSILON, &ss);
							// Dejo en ss la part entera de v/q
			return AbstractValue::to_value_ptr(Real( ss * q.value() ));
		}

		case CellValueType::tuple:
		{
			tuple_ptr<Real> t = Tuple<Real>::ptr_from_value(v);
			std::vector<Real> tv;
			for(int i = 0; i < t->size(); ++i)
			{
				Real r = (*t)[i];
				modf(r.value()/q.value() + DBL_EPSILON, &ss);
				tv.push_back(Real( ss * q.value() ));
			}

			return AbstractValue::to_value_ptr(Tuple<Real>(&tv));
		}

	}
}

Real power (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() )
		return Real();	// Devuelve un indefinido

	if ( r1.value() < 0 || fractional(r2).value() != 0)
		return Real();	// Si r1 es negativo o r2 no es entero

	return Real( pow( r1.value(), r2.value() ) );
}

Real max (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() )
		return Real::tundef;

	return Real( r1.value() > r2.value() ? r1.value() : r2.value() );
}

Real min (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() )
		return Real::tundef;

	return Real( r1.value() < r2.value() ? r1.value() : r2.value() );
}

Real abs (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( fabs( r.value() ) );
}

Real exp (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( exp( r.value() ) );
}

Real ln (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( r.value() <= 0 )
		return Real();

	return Real( log( r.value() ) );
}

Real log (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( r.value() <= 0 )
		return Real();

	return Real( log10( r.value() ) );
}

Real atanh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( fabs( r.value() ) > 1 )
		return Real();

	return Real( atanh( r.value() ) );
}

Real atan (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( atan( r.value() ) );
}

Real asinh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( asinh( r.value() ) );
}

Real asin (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( fabs (r.value()) > 1 )
		return Real();

	return Real( asin( r.value() ) );
}

Real acosh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( r.value() < 1 )
		return Real();

	return Real( acosh( r.value() ) );
}

Real acos (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( fabs (r.value()) > 1 )
		return Real();

	return Real( acos( r.value() ) );
}

Real sign (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( r.value() > 0 )
		return Real(1);
	if ( r.value() < 0 )
		return Real(-1);

	return Real( 0 );
}

TValBool isInt (const Real &r)
{
	if ( r.IsUndefined() )
		return TValBool::tfalse;

	Real	w( r - trunc(r) );

	if ( w.value() > 0)
		return TValBool::tfalse;

	return TValBool::ttrue;	
}

TValBool even (const Real &r)
{
	return isInt( r / Real(2) );
}

TValBool odd (const Real &r)
{
	if ( isInt(r) == TValBool::tfalse )
		return TValBool::tfalse;       // es indefinido o no es entero

	if ( even(r) == TValBool::ttrue )
		return TValBool::tfalse;

	return TValBool::ttrue;		// es entero y no es par
}

TValBool isPrime (const Real &r)
{
	if ( isInt(r) == TValBool::tfalse )
		return TValBool::tfalse;    // si es indefinido o no es entero

	Value	num = r.value(), aux = 3, aux2;

	if ( fmod( num, 2 ) == 0 )	// Si 2 divide a num
		return TValBool::tfalse;

	aux2 = sqrt(num);

	while ( aux <= aux2   &&   fmod( num, aux ) != 0 )
		aux += 2;

	return aux > aux2 ? TValBool::ttrue : TValBool::tfalse;
}

Real logn (const Real &v, const Real &n)
{
	if ( v.IsUndefined() || n.IsUndefined() )
		return Real();	// Devuelve un indefinido

	if (v.value() <= 0 || n.value() <= 0)
		return Real();

	return Real( log(v.value()) / log(n.value()) );
}

Real root (const Real &v, const Real &n)
{
	if ( v.IsUndefined() || n.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( v.value() < 0 || n.value() == 0 )
		return Real();
	if ( v.value() == 0 )
		return Real(0);

	return Real( exp( (1 / n.value()) * log( v.value() ) ) );
}

Real fact (const Real &n)
{
	if ( isInt(n) == TValBool::tfalse )
		return Real();	// Devuelve un indefinido

	if ( n.value() < 0 )
		return Real();

	register long ff = 1;
	for (register int a=2; a <= n.value(); a++){
		ff *= a;

		if (ff < 0)	// Hubo Overflow
			return Real(M_INFINITO);
	}
	return Real(ff);
}

Real comb (const Real &m, const Real &k)
{
	// Aqui el combinatorio |m| = (m / k) * (m-1 / k-1) * ... (m-k+1 / 1)
	//                      |k|

	if ( isInt(m) == TValBool::tfalse || isInt(k) == TValBool::tfalse )
		return Real();	// Devuelve un indefinido

	if ( m.value() < k.value() || m.value() <= 0)
		return Real();

	register double aux = 1;

	for (register double w = 0; w <= k.value() - 1; w++){
		aux *= (m.value() - w) / (k.value() - w);

		if (aux < 0)		// Se produjo Overflow
			return Real(M_INFINITO);
	}
	return Real(aux);
}

Real random(const Real &r)
{
	if ( r.IsUndefined() )
		return Real::tundef;

	return uniform( Real::zero, Real::one );
}

Real chi(const Real &r)
{
	if ( r.IsUndefined() || r.value() <= 0 )
		return Real::tundef;

	return Real( genchi( r.value() ) );
}

Real beta(const Real &a, const Real &b)
{
	if ( a.IsUndefined() || b.IsUndefined() )
		return Real::tundef;
	if ( a.value() < 1e-37 || b.value() < 1e-37)
		return Real::tundef;

	return Real( genbet( a.value(), b.value() ) );
}

Real exponential(const Real &r)
{
	if ( r.IsUndefined() || r.value() < 0 )
		return Real::tundef;

	return Real( genexp( r.value() ) );
}

Real f(const Real &a, const Real &b)
{
	if ( a.IsUndefined() || b.IsUndefined() )
		return Real::tundef;
	if ( a.value() <= 0 || b.value() <= 0)
		return Real::tundef;

	return Real( genf( a.value(), b.value() ) );
}

Real gamma(const Real &a, const Real &b)
{
	if ( a.IsUndefined() || b.IsUndefined() )
		return Real::tundef;
	if ( a.value() <= 0 || b.value() <= 0)
		return Real::tundef;

	return Real( gengam( a.value(), b.value() ) );
}

Real normal(const Real &a, const Real &b)
{
	if ( a.IsUndefined() || b.IsUndefined() )
		return Real::tundef;
	if ( b.value() < 0)
		return Real::tundef;

	return Real( gennor( a.value(), b.value() ) );
}

Real uniform(const Real &a, const Real &b)
{
	if ( a.IsUndefined() || b.IsUndefined() )
		return Real::tundef;
	if ( a.value() > b.value() )
		return Real::tundef;

	return Real( genunf( a.value(), b.value() ) );
}

Real randInt(const Real &n)
{
	return round( uniform(0,n) );
}

Real binomial(const Real &n, const Real &p)
{
	if ( n.IsUndefined() || p.IsUndefined() )
		return Real::tundef;
	if ( n.value() < 0 || !isInt(n) )
		return Real::tundef;
	if ( p.value() > 1 || p.value() < 0)
		return Real::tundef;

	return Real( ignbin( (long) n.value(), p.value() ) );
}

Real poisson(const Real &m)
{
	if ( m.IsUndefined() || m.value() < 0 )
		return Real::tundef;

	return Real( ignpoi( m.value() ) );
}

Real mcm (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() )
		return Real::tundef;
	if ( !isInt(r1) || !isInt(r2) )
		return Real::tundef;

	register long	t, r, m, d, a = (long) r1.value(), b = (long) r2.value();

	if (b >= a)
		t=a, a=b, b=t;		// Swap(a,b)
	if ( (float) a / b == (long)(a/b))
		return Real(a);
	d = (long) a/b;
	do {
		d++;
		m = d * b;
		r = m - a * (long)(m/a);
	} while (r != 0);
	return Real(m);
}

Real gcd (const Real &r1, const Real &r2)
{
	if ( r1.IsUndefined() || r2.IsUndefined() )
		return Real::tundef;

	if ( !isInt(r1) || !isInt(r2) )
		return Real::tundef;

	register long	t, q, r, k = 0, a = (long) r1.value(), b = (long) r2.value();

	if (b >= a)
		t=a, a=b, b=t;		// Swap(a,b)

	while ( k == 0){
		q = (long) (a/b);
		r = (long) (a-b*q);
		if (r == 0)
			k = 1;
		a = b;
		b = r;
	}
	return Real(a);
}

Real hip (const Real &c1, const Real &c2)
{
	if ( c1.IsUndefined() || c2.IsUndefined() )
		return Real::tundef;

	if (c1.value() < 0 || c2.value() < 0)
		return Real::tundef;

	return Real( sqrt( c1.value() * c1.value() + c2.value() * c2.value() ) );
}

Real rectToPolar_r (const Real &x, const Real &y)
{
	Real c1( abs(x) ), c2( abs(y) );
	return hip(c1,c2);
}

Real rectToPolar_angle (const Real &x, const Real &y)
{
	return atan( y / x );
}

Real polarToRect_x (const Real &r, const Real &angle)
{
	if ( r.IsUndefined() || angle.IsUndefined() )
		return Real::tundef;

	if ( r.value() < 0 )
		return Real::tundef;

	return Real( r.value() * cos( angle.value() ) );
}

Real polarToRect_y (const Real &r, const Real &angle)
{
	if ( r.IsUndefined() || angle.IsUndefined() )
		return Real::tundef;

	if ( r.value() < 0 )
		return Real::tundef;

	return Real( r.value() * sin( angle.value() ) );
}

Real cotan (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	Real w( tan( r ) );
	if (w.value() == 0)
		return Real( M_INFINITO );

	return Real( 1 / w.value() );
}

Real cosec (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	Real w( sin( r ) );
	if (w.value() == 0)
		return Real( M_INFINITO );

	return Real( 1 / w.value() );
}

Real cosech (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	Real w( sinh( r ) );
	if (w.value() == 0)
		return Real( M_INFINITO );

	return Real( 1 / w.value() );
}

Real randomSign(const Real &r)
{
	if ( r.IsUndefined() )
		return Real::tundef;

	if ( uniform( Real(-1), Real(1) ).value() >= 0)
		return Real(1);

	return Real(-1);
}

Real degToRad(const Real &r)
{
	if ( r.IsUndefined() )
		return Real::tundef;

	return Real( r.value() * M_PI / 180 );
}

Real radToDeg(const Real &r)
{
	if ( r.IsUndefined() )
		return Real::tundef;

	return Real( r.value() * 180 / M_PI );
}

Real nextPrime (const Real &n)
{
	if ( n.IsUndefined() )
		return Real::tundef;

	Real	k( trunc( n ) );

	if (k == 2)
		return Real(3);
	else if (k < 2)
		return Real(2);

	// Sino k > 2
	if ( odd(k) == TValBool::ttrue )
		k = k + Real(2);
	else
		k = k + Real(1);

	while ( isPrime(k) != TValBool::ttrue   &&   k.value() < M_INFINITO )
		k = k + Real(2);

	return k;
}

Real nth_Prime (const Real &n)
{
	if ( n.IsUndefined() || !isInt(n) || n.value() < 1 )
		return Real::tundef;

	if (n.value() == 1)
		return Real(2);

	// Sino n >= 2
	Real	k = 3;
	register long	i = 2;

	while ( i < n.value() )
	{
		k = nextPrime( k );
		i++;

		if (k.value() == M_INFINITO)
			return Real(M_INFINITO);
	}
	return k;
}

Real realIf (const TValBool &c, const Real &t, const Real &f)
{
	if (c == TValBool::ttrue)
		return t;

	return f;
}

Real realIfu (const TValBool &c, const Real &t, const Real &f, const Real &u)
{
	if (c == TValBool::ttrue)
		return t;

	if (c == TValBool::tfalse)
		return f;

	return u;
}

Real asec (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido
	if ( fabs (r.value()) < 1 )
		return Real();

	return Real( atan ( r.value() / sqrt( r.value() * r.value() - 1) ) + sign( sign(r.value()) - 1) * M_PI/2 );
}

Real acotan (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( atan ( r.value() ) + M_PI / 2 );
}

Real asech (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	if ( r.value() == 0)
		return Real( M_INFINITO );

	return Real( log ( ( sqrt( (-r.value() * r.value() + 1) + 1 ) / r.value() ) ) );
}

Real acosech (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	if ( r.value() == 0)
		return Real( M_INFINITO );

	return Real( log ( ( sign(r.value()) * sqrt( (r.value() * r.value() + 1 ) + 1 ) / r.value() ) ) );
}

Real acotanh (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	if ( r.value() == 1)
		return Real( M_INFINITO );

	return Real( log ( (r.value() + 1) / (r.value() - 1) ) / 2 );
}

Real CtoF (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( (9 * r.value() / 5) + 32);
}

Real CtoK (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( r.value() + 273 );
}

Real KtoC (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( r.value() - 273 );
}

Real KtoF (const Real &r)
{
	return CtoF( KtoC(r) );
}

Real FtoC (const Real &r)
{
	if ( r.IsUndefined() )
		return Real();	// Devuelve un indefinido

	return Real( (r.value() - 32) * 5 / 9 );
}

Real FtoK (const Real &r)
{
	return CtoK( FtoC(r.value()) );
}

TValBool isUndefinedReal(const Real &r)
{
	if ( r.IsUndefined() )
		return TValBool::ttrue;

	return TValBool::tfalse;
}
