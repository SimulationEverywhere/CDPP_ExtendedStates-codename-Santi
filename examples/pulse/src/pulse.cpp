#include <random>
#include <string>

#include "message.h"
#include "parsimu.h"
#include "real.h"
#include "tuple_value.h"

#include "pulse.h"

using namespace std;


#define VERBOSE true

#define PRINT_TIMES(f) {\
	VTime timeleft = nextChange();\
	VTime elapsed  = msg.time() - lastChange();\
	VTime sigma    = elapsed + timeleft;\
	cout << f << "@" << msg.time() <<\
		" - timeleft: " << timeleft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}


Pulse::Pulse(const string &name) :
	Atomic(name),
	start(addInputPort("start")),
	stop(addInputPort("stop")), 
	out(addOutputPort("out")),
	frequency_time(0,0,1,0),
	on(true),
	dist(0,100),
	rng(random_device()())
{
}


Model &Pulse::initFunction()
{
	holdIn(AtomicState::active, this->frequency_time);
	return *this;
}


Model &Pulse::externalFunction(const ExternalMessage &msg)
{
#if VERBOSE
	PRINT_TIMES("dext");
#endif

	if(msg.port() == start)
	{
		this->on = true;
		holdIn(AtomicState::active, this->frequency_time);
	}
	else if(msg.port() == stop)
	{
		this->on = false;
		passivate();
	}

	return *this;
}


Model &Pulse::internalFunction(const InternalMessage &msg)
{
#if VERBOSE
	PRINT_TIMES("dint");
#endif

	if(this->on)
		holdIn(AtomicState::active, this->frequency_time);
	else
		passivate();

	return *this ;
}


Model &Pulse::outputFunction(const CollectMessage &msg)
{
	auto random_int = this->dist(this->rng);
	Tuple<Real> out_value{Real(random_int), 0, 1};
	sendOutput(msg.time(), out, out_value);
	return *this ;
}
