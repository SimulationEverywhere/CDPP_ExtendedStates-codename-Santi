#ifndef _QSS2_H_
#define _QSS2_H_

#include <string>

#include "atomic.h"
#include "qss_utils.h"
#include "VTime.h"

#define QSS2_MODEL_NAME "QSS2"


class QSS2 : public Atomic
{
public:
	QSS2(const string &name = QSS2_MODEL_NAME );
	virtual string className() const {  return QSS2_MODEL_NAME ;}

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

private:
	const Port &in;
	Port &out;

	double dQ, dQMin, dQRel;
	double x[3], q[2];

	double gain;

	VTime sigma;

	bool log_output;
	std::string logFileName;
};

#endif
