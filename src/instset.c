#include <assert.h>
#include "types.h"
#include "instset.h"

sbool
si_isInst(sword inst)
{
	return inst < SINST_COUNT;
}

static sbool
isBetween(sword inst, sword lo, sword hi)
{
	assert(si_isInst(inst));
	assert(lo < SINST_COUNT);
	assert(hi < SINST_COUNT);

	if (inst < lo) {
		return SFALSE;
	}

	if (inst > hi) {
		return SFALSE;
	}

	return STRUE;
}

sbool
si_isMod(sword inst)
{
	assert(si_isInst(inst));
	return isBetween(inst, SNOP0, SNOP3);
}

sbool
si_isKey(sword inst)
{
	assert(si_isInst(inst));
	return isBetween(inst, SKEYA, SKEYP);
}

sbool
si_isLock(sword inst)
{
	assert(si_isInst(inst));
	return isBetween(inst, SLOKA, SLOKP);
}

sbool
si_keyLockMatch(sword key, sword lock)
{
	assert(si_isKey(key));
	assert(si_isInst(lock));
	return (key - SKEYA) == (lock - SLOKA);
}
