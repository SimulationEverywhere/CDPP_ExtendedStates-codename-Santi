#include <math.h>

#include "qss_utils.h"
#include "except.h"


VTime minposroot(double *coeff, int order)
{
	switch(order)
	{
	case 1:
		return minposroot_1(coeff);
	case 2:
		return minposroot_2(coeff);
	case 3:
		return minposroot_3(coeff);
	default:
		MException e("Supplied QSS order not implemented.");
		MTHROW(e);
	}
}

VTime minposroot_1(double *coeff)
{
	VTime mpr;

	if(coeff[1] == 0)
		mpr = VTime::Inf;
	else
		mpr = VTime(-coeff[0]/coeff[1]);

	if(mpr < VTime::Zero || mpr >= VTime::Inf)
		mpr = VTime::Inf;

	return mpr;
}

VTime minposroot_2(double *coeff)
{
	VTime mpr;

	if(coeff[2] == 0 || (1000*fabs(coeff[2])) < fabs(coeff[1]))
		mpr = minposroot_1(coeff);
	else
	{
		double disc = coeff[1]*coeff[1] - 4*coeff[2]*coeff[0];

		if(disc < 0)
			mpr = VTime::Inf;
		else
		{
			double sd = sqrt(disc), r1 = (-coeff[1]+sd)/2/coeff[2];

			if(r1 > 0)
				mpr = VTime(r1);
			else
				mpr = VTime::Inf;

			r1 = (-coeff[1]-sd)/2/coeff[2];
			if((r1 > 0) && (mpr > r1))
				mpr = VTime(r1);
		}
	}

	if(mpr < 0 || mpr >= VTime::Inf)
		mpr = VTime::Inf;

	return mpr;
}

VTime minposroot_3(double *coeff)
{
	VTime mpr;

	if((coeff[3] == 0) || (1000*fabs(coeff[3]) < fabs(coeff[2])))
		mpr = minposroot_2(coeff);
	else
	{
		double q, r, disc;

		q = (3*coeff[3]*coeff[1]-coeff[2]*coeff[2])/9/coeff[3]/coeff[3];
		r = (9*coeff[3]*coeff[2]*coeff[1]-27*coeff[3]*coeff[3]*coeff[0]-2*coeff[2]*coeff[2]*coeff[2])/54/coeff[3]/coeff[3]/coeff[3];
		disc = q*q*q+r*r;

		mpr = VTime::Inf;

		if(disc >= 0)
		{
			//only one real root
			double sd, s, t, r1;

			sd = sqrt(disc);
			if(r+sd > 0)
				s = pow(r+sd,1.0/3);
			else
				s = -pow(fabs(r+sd),1.0/3);

			if(r-sd > 0)
				t = pow(r-sd,1.0/3);
			else
				t = -pow(fabs(r-sd),1.0/3);

			r1 = s+t-coeff[2]/3/coeff[3];
			if(r1 > 0)
				mpr = VTime(r1);
		}
		else
		{
			//three real roots
			double rho, th, rho13, costh3, sinth3, spt, smti32, r1;

			rho = sqrt(-q*q*q);
			th = acos(r/rho);
			rho13 = pow(rho,1.0/3);
			costh3 = cos(th/3);
			sinth3 = sin(th/3);
			spt = rho13*2*costh3;
			smti32 = -rho13*sinth3*sqrt(3);

			r1 = spt-coeff[2]/3/coeff[3];
			if(r1 > 0)
				mpr = VTime(r1);

			r1 = -spt/2-coeff[2]/3/coeff[3]+smti32;
			if((r1 > 0) && (mpr > r1))
				mpr = VTime(r1);

			r1 = r1-2*smti32;
			if((r1 > 0) && (mpr > r1))
				mpr = VTime(r1);
		}
	}

	if(mpr < 0 || mpr >= VTime::Inf)
		mpr = VTime::Inf;

	return mpr;
}

void advance_time(double *p, const VTime &dt, int order)
{
	switch(order)
	{
	case 1:
		p[0] = p[0] + dt*p[1];
		break;
	case 2:
		p[0] = p[0] + dt*p[1] + dt*dt*p[2];
		p[1] = p[1] + 2*p[2]*dt;
		break;
	case 3:
		p[0] = p[0] + dt*p[1] + dt*dt*p[2] + dt*dt*dt*p[3];
		p[1] = p[1] + 2*p[2]*dt + 3*p[3]*dt*dt;
		p[2] = p[2] + 3*p[3]*dt;
		break;
	default:
		MException e("Supplied QSS order not implemented.");
		MTHROW(e);
	}
}
