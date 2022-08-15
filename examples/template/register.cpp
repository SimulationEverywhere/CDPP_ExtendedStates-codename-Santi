#include "pmodeladm.h" 
#include "register.h"

#include "ModelTemplate.h" // class ModelName

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< ModelTemplate >(), ATOMIC_MODEL_NAME); // Register custom models
}

