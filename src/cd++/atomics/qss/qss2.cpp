#include <algorithm>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "qss2.h"

#include "parsimu.h"
#include "message.h"
#include "real.h"
#include "tuple_value.h"
#include "value.h"

using namespace std;


QSS2::QSS2(const string &name) :
	Atomic(name),
	in(addInputPort("in")),
	out(addOutputPort("out"))
{
	this->dQRel = this->get_mandatory_double_param("dQRel");
	this->dQMin = this->get_mandatory_double_param("dQMin");

	this->gain = this->get_optional_double_param("gain", 1);
	this->log_output = this->get_optional_double_param("debug", 0);

	this->x[0] = this->get_mandatory_double_param("X0");
	this->x[1] = 0;
	this->x[2] = 0;

	this->q[0] = this->x[0];
	this->q[1] = 0;

	this->dQ = max(fabs(this->x[0]) * this->dQRel, this->dQMin);

	if(this->log_output)
	{
		this->logFileName = description() + ".csv";
		ofstream outf(this->logFileName.data(), std::ofstream::out);
		outf.close();
	}
}

Model &QSS2::initFunction()
{
	this->sigma = VTime::Zero;
	holdIn(AtomicState::active, this->sigma);

	return *this;
}

Model &QSS2::externalFunction(const ExternalMessage &msg)
{
	double diffxq[3];

	Tuple<Real> derx = Tuple<Real>::from_value(msg.value());
	double derx1 = derx[0].value() * this->gain;
	double derx2 = derx[1].value() * this->gain;

	VTime e = msg.time() - lastChange();

	if(msg.port() == in)
	{
		x[0] = x[0] + x[1] * e + x[2] * e * e;
		x[1] = derx1;
		x[2] = derx2/2;

		if(this->sigma > 0)
		{
			q[0] = q[0] + q[1] * e;

		   	// inferior delta crossing
	   		diffxq[0] = q[0] - x[0] - dQ;
			diffxq[1] = q[1] - x[1];
			diffxq[2] = -x[2];
	   		this->sigma = minposroot(diffxq, 2);

	   		// superior delta difference
	   		diffxq[0] = q[0] - x[0] + dQ;
	   		VTime sigma_up = minposroot(diffxq, 2);

	   		// keep the smallest one
	   		if(sigma_up < this->sigma){
	   			this->sigma = sigma_up;
	   		}

	   		if(fabs(x[0] - q[0]) > dQ)
				sigma = VTime::Zero;
		}
	}
	else
	{
		x[0] = derx1;
		x[1] = x[1] + 2*x[2]*e;

		sigma = VTime::Zero;
	}

	holdIn(AtomicState::active, this->sigma);

	return *this;
}

Model &QSS2::internalFunction(const InternalMessage &msg)
{
	advance_time(x, this->sigma, 2);

	q[0] = x[0];
	q[1] = x[1];

	dQ = max(dQRel * fabs(x[0]), dQMin);

	if(x[2] == 0)
	{
		this->sigma = VTime::Inf;
		passivate();
	}
	else
	{
		this->sigma = VTime(sqrt(fabs(dQ/x[2])));
		holdIn(AtomicState::active, this->sigma);
	}

	return *this ;
}

Model &QSS2::outputFunction(const CollectMessage &msg)
{
	double y[3] = {x[0], x[1], x[2]};

	advance_time(y, this->sigma, 2);
	y[2] = 0;

	if(this->log_output)
	{
		// send output to file
		ofstream outf(this->logFileName.data(), std::ofstream::out | std::ofstream::app);
		outf << msg.time().asSecs() << "," << y[0] << endl;
		outf.close();
	}

	Tuple<Real> out_value{y[0], y[1], y[2]};
	sendOutput(msg.time(), out, out_value);

	return *this ;
}
