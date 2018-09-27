#ifndef _QSS1_H_
#define _QSS1_H_

#include <string>

#include "atomic.h"
#include "qss_utils.h"
#include "VTime.h"

#define QSS1_MODEL_NAME "QSS1"


class QSS1 : public Atomic
{
public:
	QSS1(const string &name = QSS1_MODEL_NAME );
	virtual string className() const {  return QSS1_MODEL_NAME ;}
  
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

private:
	const Port &in;
	Port &out;

	double dQ, dQMin, dQRel;
	double x[2], q;

	double gain;

	VTime sigma;

	bool log_output;
	std::string logFileName;
};

#endif
