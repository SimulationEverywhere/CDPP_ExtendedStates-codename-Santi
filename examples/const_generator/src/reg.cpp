#include "pmodeladm.h"
#include "register.h"

#include "constgenerat.h"


void register_atomics_on(ParallelModelAdmin &admin)
{
        admin.registerAtomic(NewAtomicFunction<ConstGenerator>(), ATOMIC_MODEL_NAME);
}
