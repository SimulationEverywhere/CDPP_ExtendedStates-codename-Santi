/*******************************************************************
*
*  DESCRIPTION: Simulator::registerNewAtomics()
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#include "pmodeladm.h" 
#include "parsimu.h"
#include "register.h"

#include "cpu.h"
#include "generat.h"
#include "queue.h"
#include "transduc.h"
#include "qss1.h"
#include "qss2.h"
#include "qss3.h"

using namespace std;


void ParallelMainSimulator::registerNewAtomics()
{
	ParallelModelAdmin &admin = SingleParallelModelAdm::Instance();

	// Registration of built-in atomic models.
	admin.registerAtomic(NewAtomicFunction<CPU>(), CPU_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<Generator>(), GENERATOR_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<Queue>(), QUEUE_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<Transducer>(), TRANSDUCER_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<QSS1>(), QSS1_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<QSS2>(), QSS2_MODEL_NAME);
	admin.registerAtomic(NewAtomicFunction<QSS3>(), QSS3_MODEL_NAME);

	// Registration of user-defined atomic models.
	register_atomics_on(admin);
}


#ifndef REGISTER_ATOMICS
void register_atomics_on(ParallelModelAdmin &admin)
{
	// This function can be used to dynamically register ad-hoc atomic models
	// defined by the user. In order to do this, the user must:
	//   * Provide a custom implementation of this function in one of their
	//     source files.
	//   * Compile the CD++ kernel defining the flag REGISTER_ATOMICS like so:
	//     DEFINES_CPP = -DREGISTER_ATOMICS
	// Note that the default behavior is to do nothing, as the kernel does not
	// need to register any custom atomic model.
}
#endif
