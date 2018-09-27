#ifndef _QSS3_H_
#define _QSS3_H_

#include <string>

#include "atomic.h"
#include "qss_utils.h"
#include "VTime.h"

#define QSS3_MODEL_NAME "QSS3"


class QSS3 : public Atomic
{
public:
	QSS3(const string &name = QSS3_MODEL_NAME );
	virtual string className() const {  return QSS3_MODEL_NAME ;}

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

private:
	const Port &in;
	Port &out;

	double dQ, dQMin, dQRel;
	double x[4], q[3];

	double gain;

	VTime sigma;

	bool log_output;
	std::string logFileName;
};

#endif
