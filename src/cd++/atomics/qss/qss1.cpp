#include <algorithm>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "qss1.h"

#include "parsimu.h"
#include "message.h"
#include "real.h"
#include "tuple_value.h"
#include "value.h"

using namespace std;


QSS1::QSS1(const string &name) :
	Atomic(name),
	in(addInputPort("in")),
	out(addOutputPort("out"))
{
	this->gain = this->get_optional_double_param("gain", 1);
	this->log_output = this->get_optional_double_param("debug", 0);

	this->dQRel = this->get_mandatory_double_param("dQRel");
	this->dQMin = this->get_mandatory_double_param("dQMin");

	this->x[0] = this->get_mandatory_double_param("X0");
	this->x[1] = 0;

	this->q = this->x[0];

	this->dQ = max(fabs(this->x[0]) * this->dQRel, this->dQMin);

	if(this->log_output)
	{
		this->logFileName = description() + ".csv";
		ofstream outf(this->logFileName.data(), std::ofstream::out);
		outf.close();
	}
}

Model &QSS1::initFunction()
{
	this->sigma = VTime::Zero;
	holdIn(AtomicState::active, this->sigma);

	return *this;
}

Model &QSS1::externalFunction(const ExternalMessage &msg)
{
	double diffxq[2];

	Real derx = Tuple<Real>::from_value(msg.value())[0];
	double derx_val = derx.value() * this->gain;

	VTime e = msg.time() - lastChange();

	if(msg.port() == in)
	{
		x[0] = x[0] + x[1] * e;
		x[1] = derx_val;

		if(this->sigma > 0)
		{
		   	// inferior delta crossing
			diffxq[1] = -x[1];
	   		diffxq[0] = q - x[0] -dQ;
	   		this->sigma = minposroot(diffxq, 1);

	   		// superior delta difference
	   		diffxq[0] = q - x[0] + dQ;
	   		VTime sigma_up = minposroot(diffxq, 1);

	   		// keep the smallest one
	   		if(sigma_up < this->sigma)
	   			this->sigma = sigma_up;

	   		if(fabs(x[0] - q) > dQ)
				sigma = VTime::Zero;
		}
	}
	else
	{
		x[0] = derx_val;
		sigma = VTime::Zero;
	}

	holdIn(AtomicState::active, this->sigma);

	return *this;
}

Model &QSS1::internalFunction(const InternalMessage &msg)
{
	advance_time(x, this->sigma, 1);
	q = x[0];

	dQ = max(dQRel * fabs(x[0]), dQMin);

	if(x[1] == 0)
	{
		this->sigma = VTime::Inf;
		passivate();
	}
	else
	{
		this->sigma = VTime(fabs(dQ/x[1]));
		holdIn(AtomicState::active, this->sigma);
	}

	return *this;
}

Model &QSS1::outputFunction(const CollectMessage &msg)
{
	double y[2] = {x[0], x[1]};

	advance_time(y, this->sigma, 1);
	y[1] = 0;

	if(this->log_output)
	{
		// send output to file
		ofstream outf(this->logFileName.data(), std::ofstream::out | std::ofstream::app);
		outf << msg.time().asSecs() << "," << y[0] << endl;
		outf.close();
	}

	Tuple<Real> out_value{y[0], y[1]};
	sendOutput(msg.time(), out, out_value);

	return *this;
}
