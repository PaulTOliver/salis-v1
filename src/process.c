#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "instset.h"
#include "memory.h"
#include "process.h"
#include "evolver.h"

static sbool  g_isInit;
static sword  g_count;
static sword  g_cap;
static sword  g_first;
static sword  g_last;
static SProc *g_procs;

void
sp_init(void)
{
	assert(!g_isInit);
	g_isInit = STRUE;
	g_cap    = 1;
	g_first  = SNULL;
	g_last   = SNULL;
	g_procs  = calloc(g_cap, sizeof(SProc));
	assert(g_procs);
}

void
sp_quit(void)
{
	assert(g_isInit);
	free(g_procs);
	g_isInit = SFALSE;
	g_count  = 0;
	g_cap    = 0;
	g_first  = 0;
	g_last   = 0;
	g_procs  = NULL;
}

void
sp_load(FILE *file)
{
	assert(!g_isInit);
	assert(file);
	fread(&g_isInit, sizeof(sbool), 1, file);
	fread(&g_count,  sizeof(sword), 1, file);
	fread(&g_cap,    sizeof(sword), 1, file);
	fread(&g_first,  sizeof(sword), 1, file);
	fread(&g_last,   sizeof(sword), 1, file);
	g_procs = calloc(g_cap, sizeof(SProc));
	assert(g_procs);
	fread(g_procs, sizeof(SProc), g_cap, file);
}

void
sp_save(FILE *file)
{
	assert(g_isInit);
	assert(file);
	fwrite(&g_isInit, sizeof(sbool), 1,     file);
	fwrite(&g_count,  sizeof(sword), 1,     file);
	fwrite(&g_cap,    sizeof(sword), 1,     file);
	fwrite(&g_first,  sizeof(sword), 1,     file);
	fwrite(&g_last,   sizeof(sword), 1,     file);
	fwrite(g_procs,   sizeof(SProc), g_cap, file);
}

sbool
sp_isInit(void)
{
	return g_isInit;
}

sword
sp_getCount(void)
{
	return g_count;
}

sword
sp_getCap(void)
{
	return g_cap;
}

sword
sp_getFirst(void)
{
	return g_first;
}

sword
sp_getLast(void)
{
	return g_last;
}

sbool
sp_isFree(sword pidx)
{
	sbool isFree;
	assert(g_isInit);
	assert(pidx < g_cap);
	isFree = !(g_procs[pidx].mb1s);
#ifndef NDEBUG

	if (isFree) {
		assert(!g_procs[pidx].mb1a);
		assert(!g_procs[pidx].mb2a);
		assert(!g_procs[pidx].mb2s);
	}

#endif
	return isFree;
}

SProc
sp_getProc(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	return g_procs[pidx];
}

void
sp_setProc(sword pidx, SProc proc)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	g_procs[pidx] = proc;
}

static void
reallocQueue(sword queueLock)
{
	sword  newCap;
	SProc *newQueue;
	sword  fidx;
	sword  bidx;
	assert(g_isInit);
	assert(g_count == g_cap);
	assert(queueLock < g_cap);
	newCap   = g_cap * 2;
	newQueue = calloc(newCap, sizeof(SProc));
	assert(newQueue);
	fidx = queueLock;
	bidx = (queueLock - 1) % newCap;

	/* copy all forward from lock */
	while (STRUE) {
		sword oldIdx = fidx % g_cap;
		memcpy(&newQueue[fidx], &g_procs[oldIdx], sizeof(SProc));

		if (oldIdx == g_last) {
			g_last = fidx;
			break;
		} else {
			fidx++;
		}
	}

	if (queueLock != g_first) {
		/* copy all backward from lock */
		while (STRUE) {
			sword oldIdx = bidx % g_cap;
			memcpy(&newQueue[bidx], &g_procs[oldIdx], sizeof(SProc));

			if (oldIdx == g_first) {
				g_first = bidx;
				break;
			} else {
				bidx--;
				bidx %= newCap;
			}
		}
	}

	free(g_procs);
	g_cap   = newCap;
	g_procs = newQueue;
}

static sword
getNewProc(sword queueLock)
{
	assert(g_isInit);

	if (g_count == g_cap) {
		reallocQueue(queueLock);
	}

	g_count++;

	if (g_count == 1) {
		g_first = 0;
		g_last  = 0;
		return 0;
	} else {
		g_last++;
		g_last %= g_cap;
		return g_last;
	}
}

static void
create(sword addr, sword size, sword queueLock, sbool allocate)
{
	sword pidx;
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	assert(sm_isValidAt(addr + size - 1));

	if (allocate) {
		sword offset;

		for (offset = 0; offset < size; offset++) {
			sword naddr = addr + offset;
			assert(!sm_isAllocatedAt(naddr));
			assert(!sm_isMemBlockStartAt(naddr));
			sm_allocateAt(naddr);
		}

		sm_setMemBlockStartAt(addr);
	}

	pidx               = getNewProc(queueLock);
	g_procs[pidx].mb1a = addr;
	g_procs[pidx].mb1s = size;
	g_procs[pidx].ip   = addr;
	g_procs[pidx].sp   = addr;
}

void
sp_create(sword addr, sword size)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	assert(sm_isValidAt(addr + size - 1));
	create(addr, size, 0, STRUE);
}

static void
freeMemBlock(sword addr, sword size)
{
	sword offset;
	assert(sm_isValidAt(addr));
	assert(sm_isValidAt(addr + size - 1));
	assert(size);
	assert(sm_isMemBlockStartAt(addr));
	sm_unsetMemBlockStartAt(addr);

	for (offset = 0; offset < size; offset++) {
		sword oaddr = addr + offset;
		assert(sm_isValidAt(oaddr));
		assert(sm_isAllocatedAt(oaddr));
		assert(!sm_isMemBlockStartAt(oaddr));
		sm_freeAt(oaddr);
	}
}

static void
freeMemOwnedBy(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	freeMemBlock(g_procs[pidx].mb1a, g_procs[pidx].mb1s);

	if (g_procs[pidx].mb2s) {
		assert(g_procs[pidx].mb1a != g_procs[pidx].mb2a);
		freeMemBlock(g_procs[pidx].mb2a, g_procs[pidx].mb2s);
	}
}

void
sp_kill(void)
{
	assert(g_isInit);
	assert(g_count);
	assert(g_first != SNULL);
	assert(g_last  != SNULL);
	assert(!sp_isFree(g_first));
	freeMemOwnedBy(g_first);
	memset(&g_procs[g_first], 0, sizeof(SProc));
	g_count--;

	if (g_first == g_last) {
		g_first = SNULL;
		g_last  = SNULL;
		return;
	}

	g_first++;
	g_first %= g_cap;
}

static void
incrementIP(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	g_procs[pidx].ip++;
	g_procs[pidx].sp = g_procs[pidx].ip;
}

static void
onFault(sword pidx)
{
	sword ip;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	ip = sp_getProc(pidx).ip;
	assert(sm_isValidAt(ip));
	se_randomizeAt(ip);
}

static sbool
seek(sword pidx, sbool forward)
{
	sword nextAddr;
	sbyte nextInst;
	sbyte spInst;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	nextAddr = g_procs[pidx].ip + 1;

	if (!sm_isValidAt(nextAddr)) {
		onFault(pidx);
		incrementIP(pidx);
		return SFALSE;
	}

	nextInst = sm_getInstAt(nextAddr);

	if (!si_isKey(nextInst)) {
		onFault(pidx);
		incrementIP(pidx);
		return SFALSE;
	}

	spInst = sm_getInstAt(g_procs[pidx].sp);

	if (si_keyLockMatch(nextInst, spInst)) {
		return STRUE;
	}

	if (forward) {
		g_procs[pidx].sp++;
	} else {
		g_procs[pidx].sp--;
	}

	return SFALSE;
}

static void
jump(sword pidx)
{
#ifndef NDEBUG
	sbyte nextInst;
	sbyte spInst;
#endif
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
#ifndef NDEBUG
	nextInst = sm_getInstAt(g_procs[pidx].ip + 1);
	spInst   = sm_getInstAt(g_procs[pidx].sp);
	assert(si_isKey(nextInst));
	assert(si_isLock(spInst));
	assert(si_keyLockMatch(nextInst, spInst));
#endif
	g_procs[pidx].ip = g_procs[pidx].sp;
}

static sword *
getRegAddr(sword pidx, sword modAddr)
{
	sbyte modInst;
	sbyte modOffset;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	assert(sm_isValidAt(modAddr));
	modInst = sm_getInstAt(modAddr);
	assert(si_isMod(modInst));
	modOffset = modInst - SNOP0;
	assert(modOffset < SPROC_REG_COUNT);
	return &(g_procs[pidx].regs[modOffset]);
}

static void
getRegAddrList(sword pidx, sword **regList, sword regCount, sbool offset)
{
	sword modAddr;
	sbyte ridx;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	assert(regList);
	assert(regCount);
	assert(regCount < 4);

	if (offset) {
		modAddr = g_procs[pidx].ip + 2;
	} else {
		modAddr = g_procs[pidx].ip + 1;
	}

	for (ridx = 0; ridx < regCount; ridx++) {
		regList[ridx] = &(g_procs[pidx].regs[0]);
	}

	for (ridx = 0; ridx < regCount; ridx++) {
		sword modNext = modAddr + ridx;

		if (!sm_isValidAt(modNext)) {
			break;
		} else {
			sword modInst = sm_getInstAt(modNext);

			if (!si_isMod(modInst)) {
				break;
			} else {
				regList[ridx] = getRegAddr(pidx, modNext);
			}
		}
	}
}

static void
addr(sword pidx)
{
#ifndef NDEBUG
	sbyte nextInst;
	sbyte spInst;
#endif
	sword *reg;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
#ifndef NDEBUG
	nextInst = sm_getInstAt(g_procs[pidx].ip + 1);
	spInst   = sm_getInstAt(g_procs[pidx].sp);
	assert(si_isKey(nextInst));
	assert(si_isLock(spInst));
	assert(si_keyLockMatch(nextInst, spInst));
#endif
	getRegAddrList(pidx, &reg, 1, STRUE);
	*reg = g_procs[pidx].sp;
	incrementIP(pidx);
}

static void
ifnz(sword pidx)
{
	sword *reg;
	sword  jumpMod;
	sword  nextAddr;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, &reg, 1, SFALSE);
	nextAddr = g_procs[pidx].ip + 1;

	if (sm_isValidAt(nextAddr)) {
		sbyte nextInst      = sm_getInstAt(nextAddr);
		sbool nextInstIsMod = si_isMod(nextInst);

		if (nextInstIsMod) {
			jumpMod = 1;
		} else {
			jumpMod = 0;
		}
	} else {
		jumpMod = 0;
	}

	if (*reg) {
		/* execute next instruction */
		g_procs[pidx].ip += (jumpMod + 1);
	} else {
		/* skip next instruction */
		g_procs[pidx].ip += (jumpMod + 2);
	}

	g_procs[pidx].sp = g_procs[pidx].ip;
}

static void
freeMemBlock2Of(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	assert(g_procs[pidx].mb2s);
	freeMemBlock(g_procs[pidx].mb2a, g_procs[pidx].mb2s);
	g_procs[pidx].mb2a = 0;
	g_procs[pidx].mb2s = 0;
}

static void
alloc(sword pidx, sbool forward)
{
	sword *regs[2];
	sword  blockSize;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, regs, 2, SFALSE);
	blockSize = *regs[0];

	/* check for possible errors */
	/* ignore if requested block size is zero */
	if (!blockSize) {
		incrementIP(pidx);
		return;
	}

	/* ignore if sp is not adjacent to already existing memory block */
	if (g_procs[pidx].mb2s) {
		sword correctAddr = g_procs[pidx].mb2a;

		if (forward) {
			correctAddr += g_procs[pidx].mb2s;
		} else {
			correctAddr--;
		}

		if (g_procs[pidx].sp != correctAddr) {
			onFault(pidx);
			incrementIP(pidx);
			return;
		}
	}

	/* on successful allocation */
	/* increment ip and save new block's address on register */
	if (g_procs[pidx].mb2s == blockSize) {
		incrementIP(pidx);
		*regs[1] = g_procs[pidx].mb2a;
		return;
	}

	/* handle block enlargement */
	/* handle sp collision with allocated space */
	if (sm_isAllocatedAt(g_procs[pidx].sp)) {
		if (g_procs[pidx].mb2s) {
			freeMemBlock2Of(pidx);
		}

		if (forward) {
			g_procs[pidx].sp++;
		} else {
			g_procs[pidx].sp--;
		}

		return;
	}

	/* enlarge block when no collision occurs */
	sm_allocateAt(g_procs[pidx].sp);

	/* correct memory block start flag address */
	if (!g_procs[pidx].mb2s) {
		g_procs[pidx].mb2a = g_procs[pidx].sp;
		sm_setMemBlockStartAt(g_procs[pidx].sp);
	} else {
		if (!forward) {
			sm_unsetMemBlockStartAt(g_procs[pidx].mb2a);
			g_procs[pidx].mb2a = g_procs[pidx].sp;
			sm_setMemBlockStartAt(g_procs[pidx].mb2a);
		}
	}

	g_procs[pidx].mb2s++;

	/* move sp to next location */
	if (forward) {
		g_procs[pidx].sp++;
	} else {
		g_procs[pidx].sp--;
	}
}

static void
bswap(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));

	if (g_procs[pidx].mb2s) {
		sword addrTmp      = g_procs[pidx].mb1a;
		sword sizeTmp      = g_procs[pidx].mb1s;
		g_procs[pidx].mb1a = g_procs[pidx].mb2a;
		g_procs[pidx].mb1s = g_procs[pidx].mb2s;
		g_procs[pidx].mb2a = addrTmp;
		g_procs[pidx].mb2s = sizeTmp;
	} else {
		onFault(pidx);
	}

	incrementIP(pidx);
}

static void
bclear(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));

	if (g_procs[pidx].mb2s) {
		freeMemBlock2Of(pidx);
	} else {
		onFault(pidx);
	}

	incrementIP(pidx);
}

static void
split(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));

	if (g_procs[pidx].mb2s) {
		create(g_procs[pidx].mb2a, g_procs[pidx].mb2s, pidx, SFALSE);
		g_procs[pidx].mb2a = 0;
		g_procs[pidx].mb2s = 0;
	} else {
		onFault(pidx);
	}

	incrementIP(pidx);
}

static void
r3op(sword pidx, sbyte inst)
{
	sword *regs[3];
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, regs, 3, SFALSE);

	/* fault when dividing by zero */
	if ((inst == SDIVN) && (*regs[2] == 0)) {
		onFault(pidx);
		incrementIP(pidx);
		return;
	}

	switch (inst) {
	case SADDN:
		*regs[0] = *regs[1] + *regs[2];
		break;

	case SSUBN:
		*regs[0] = *regs[1] - *regs[2];
		break;

	case SMULN:
		*regs[0] = *regs[1] * *regs[2];
		break;

	case SDIVN:
		assert(*regs[2]);
		*regs[0] = *regs[1] / *regs[2];
		break;

	default:
		assert(0);
	}

	incrementIP(pidx);
}

static void
r1op(sword pidx, sbyte inst)
{
	sword *reg;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, &reg, 1, SFALSE);

	switch (inst) {
	case SINCN:
		(*reg)++;
		break;

	case SDECN:
		(*reg)--;
		break;

	case SNOTN:
		*reg = !(*reg);
		break;

	case SSHFL:
		*reg <<= 1;
		break;

	case SSHFR:
		*reg >>= 1;
		break;

	case SZERO:
		*reg = 0;
		break;

	case SUNIT:
		*reg = 1;
		break;

	default:
		assert(0);
	}

	incrementIP(pidx);
}

static void
push(sword pidx)
{
	sword *reg;
	sword  sidx;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, &reg, 1, SFALSE);

	for (sidx = SPROC_STACK_SIZE - 1; sidx; sidx--) {
		g_procs[pidx].stack[sidx] = g_procs[pidx].stack[sidx - 1];
	}

	g_procs[pidx].stack[0] = *reg;
	incrementIP(pidx);
}

static void
pop(sword pidx)
{
	sword *reg;
	sword  sidx;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, &reg, 1, SFALSE);
	*reg = g_procs[pidx].stack[0];

	for (sidx = 1; sidx < SPROC_STACK_SIZE; sidx++) {
		g_procs[pidx].stack[sidx - 1] = g_procs[pidx].stack[sidx];
	}

	g_procs[pidx].stack[SPROC_STACK_SIZE - 1] = 0;
	incrementIP(pidx);
}

static void
load(sword pidx)
{
	sword *regs[2];
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, regs, 2, SFALSE);

	if (!sm_isValidAt(*regs[0])) {
		onFault(pidx);
		incrementIP(pidx);
		return;
	}

	if (g_procs[pidx].sp < *regs[0]) {
		g_procs[pidx].sp++;
	} else if (g_procs[pidx].sp > *regs[0]) {
		g_procs[pidx].sp--;
	} else {
		*regs[1] = sm_getInstAt(*regs[0]);
		incrementIP(pidx);
	}
}

static sbool
isWriteableBy(sword pidx, sword addr)
{
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));

	if (!sm_isValidAt(addr)) {
		return SFALSE;
	}

	if (!sm_isAllocatedAt(addr)) {
		return STRUE;
	} else {
		sword lo1 = g_procs[pidx].mb1a;
		sword lo2 = g_procs[pidx].mb2a;
		sword hi1 = lo1 + g_procs[pidx].mb1s;
		sword hi2 = lo2 + g_procs[pidx].mb2s;
		return (addr >= lo1 && addr < hi1) || (addr >= lo2 && addr < hi2);
	}
}

static void
write(sword pidx)
{
	sword *regs[2];
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, regs, 2, SFALSE);

	if (!sm_isValidAt(*regs[0])) {
		onFault(pidx);
		incrementIP(pidx);
		return;
	}

	if (!si_isInst(*regs[1])) {
		onFault(pidx);
		incrementIP(pidx);
		return;
	}

	if (g_procs[pidx].sp < *regs[0]) {
		g_procs[pidx].sp++;
	} else if (g_procs[pidx].sp > *regs[0]) {
		g_procs[pidx].sp--;
	} else {
		if (isWriteableBy(pidx, *regs[0])) {
			sm_setInstAt(*regs[0], *regs[1]);
		} else {
			onFault(pidx);
		}

		incrementIP(pidx);
	}
}

static void
r2op(sword pidx, sbyte inst)
{
	sword *regs[2];
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	getRegAddrList(pidx, regs, 2, SFALSE);

	switch (inst) {
	case SDUPL:
		*regs[1] = *regs[0];
		break;

	case SSWAP: {
		sword temp = *regs[0];
		*regs[0]   = *regs[1];
		*regs[1]   = temp;
		break;
	}

	default:
		assert(0);
	}

	incrementIP(pidx);
}

static void
cycle(sword pidx)
{
	sbyte inst;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));

	if (!sm_isValidAt(g_procs[pidx].ip) || !sm_isValidAt(g_procs[pidx].sp)) {
		incrementIP(pidx);
		return;
	}

	inst = sm_getInstAt(g_procs[pidx].ip);

	switch (inst) {
	case SJMPB:
		if (seek(pidx, SFALSE)) {
			jump(pidx);
		}

		break;

	case SJMPF:
		if (seek(pidx, STRUE)) {
			jump(pidx);
		}

		break;

	case SADRB:
		if (seek(pidx, SFALSE)) {
			addr(pidx);
		}

		break;

	case SADRF:
		if (seek(pidx, STRUE)) {
			addr(pidx);
		}

		break;

	case SIFNZ:
		ifnz(pidx);
		break;

	case SALLB:
		alloc(pidx, SFALSE);
		break;

	case SALLF:
		alloc(pidx, STRUE);
		break;

	case SBSWP:
		bswap(pidx);
		break;

	case SBCLR:
		bclear(pidx);
		break;

	case SSPLT:
		split(pidx);
		break;

	case SADDN:
	case SSUBN:
	case SMULN:
	case SDIVN:
		r3op(pidx, inst);
		break;

	case SINCN:
	case SDECN:
	case SNOTN:
	case SSHFL:
	case SSHFR:
	case SZERO:
	case SUNIT:
		r1op(pidx, inst);
		break;

	case SPSHN:
		push(pidx);
		break;

	case SPOPN:
		pop(pidx);
		break;

	case SLOAD:
		load(pidx);
		break;

	case SWRTE:
		write(pidx);
		break;

	case SDUPL:
	case SSWAP:
		r2op(pidx, inst);
		break;

	default:
		incrementIP(pidx);
	}
}

#ifndef NDEBUG

static void
isFreeValid(sword pidx)
{
	sword *element;
	sword  eidx;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(sp_isFree(pidx));
	element = (sword *)&g_procs[pidx];

	for (eidx = 0; eidx < SPROC_ELEM_COUNT; eidx++) {
		assert(!(*element));
		element++;
	}
}

static void
isUsedValid(sword pidx)
{
	sword offset;
	assert(g_isInit);
	assert(pidx < g_cap);
	assert(!sp_isFree(pidx));
	assert(sm_isMemBlockStartAt(g_procs[pidx].mb1a));

	if (g_procs[pidx].mb2s) {
		assert(sm_isMemBlockStartAt(g_procs[pidx].mb2a));
	}

	for (offset = 0; offset < g_procs[pidx].mb1s; offset++) {
		sword addr = g_procs[pidx].mb1a + offset;
		assert(sm_isValidAt(addr));
		assert(sm_isAllocatedAt(addr));
	}

	for (offset = 0; offset < g_procs[pidx].mb2s; offset++) {
		sword addr = g_procs[pidx].mb2a + offset;
		assert(sm_isValidAt(addr));
		assert(sm_isAllocatedAt(addr));
	}
}

static void
isValid(sword pidx)
{
	assert(g_isInit);
	assert(pidx < g_cap);

	if (sp_isFree(pidx)) {
		isFreeValid(pidx);
	} else {
		isUsedValid(pidx);
	}
}

#endif

void
sp_cycle(void)
{
#ifndef NDEBUG
	sword pidx;
#endif
	assert(g_isInit);
#ifndef NDEBUG

	/* check for validity before cycle */
	for (pidx = 0; pidx < sp_getCap(); pidx++) {
		isValid(pidx);
	}

#endif

	if (sp_getCount()) {
		sword qidx = sp_getLast();

		/* cycle all procs */
		while (STRUE) {
			cycle(qidx);

			if (qidx == sp_getFirst()) {
				break;
			} else {
				qidx--;
				qidx %= sp_getCap();
			}
		}

		/* kill procs if memory is over capacity */
		while (sm_isOverCap()) {
			sp_kill();
		}
	} else {
		sp_create(0, 1);
	}

#ifndef NDEBUG

	/* check for validity after cycle */
	for (pidx = 0; pidx < sp_getCap(); pidx++) {
		isValid(pidx);
	}

#endif
}
