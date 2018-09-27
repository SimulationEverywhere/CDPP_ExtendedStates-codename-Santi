#include "pmodeladm.h"
#include "register.h"

#include "pulse.h"


void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction<Pulse>(), ATOMIC_MODEL_NAME);
}
