#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "instset.h"
#include "memory.h"
#include "evolver.h"

sbool g_isInit;
sword g_lastAddress;
sbyte g_lastInst;
sword g_state[4];

void
se_init(void)
{
	assert(!g_isInit);
	srand((unsigned)time(NULL));
	g_state[0] = rand();
	g_state[1] = rand();
	g_state[2] = rand();
	g_state[3] = rand();
	g_isInit   = STRUE;
}

void
se_quit(void)
{
	assert(g_isInit);
	g_isInit      = SFALSE;
	g_lastAddress = 0;
	g_lastInst    = 0;
	g_state[0]    = 0;
	g_state[1]    = 0;
	g_state[2]    = 0;
	g_state[3]    = 0;
}

void
se_load(FILE *file)
{
	assert(!g_isInit);
	assert(file);
	fread(&g_isInit,      sizeof(sbool), 1, file);
	fread(&g_lastAddress, sizeof(sword), 1, file);
	fread(&g_lastInst,    sizeof(sbyte), 1, file);
	fread(g_state,        sizeof(sword), 4, file);
}

void
se_save(FILE *file)
{
	assert(g_isInit);
	assert(file);
	fwrite(&g_isInit,      sizeof(sbool), 1, file);
	fwrite(&g_lastAddress, sizeof(sword), 1, file);
	fwrite(&g_lastInst,    sizeof(sbyte), 1, file);
	fwrite(g_state,        sizeof(sword), 4, file);
}

sbool
se_isInit(void)
{
	return g_isInit;
}

sword
se_getLastAddress(void)
{
	return g_lastAddress;
}

sbyte
se_getLastInst(void)
{
	assert(si_isInst(g_lastInst));
	return g_lastInst;
}

sword
se_getState(sword eidx)
{
	assert(eidx < 4);
	return g_state[eidx];
}

void
se_setState(sword eidx, sword state)
{
	assert(g_isInit);
	assert(eidx < 4);
	g_state[eidx] = state;
}

static sword
generateRandomNumber(void)
{
	sword s;
	sword t;
	assert(g_isInit);
	t          = g_state[3];
	t         ^= t << 11;
	t         ^= t >> 8;
	g_state[3] = g_state[2];
	g_state[2] = g_state[1];
	g_state[1] = g_state[0];
	s          = g_state[0];
	t         ^= s;
	t         ^= s >> 19;
	g_state[0] = t;
	return t;
}

void
se_randomizeAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	sm_setInstAt(addr, (sbyte)(generateRandomNumber() % SINST_COUNT));
}

void
se_cycle(void)
{
	assert(g_isInit);
	g_lastAddress = generateRandomNumber();
	g_lastInst    = (sbyte)(generateRandomNumber() % SINST_COUNT);

	if (sm_isValidAt(g_lastAddress)) {
		sm_setInstAt(g_lastAddress, (sbyte)g_lastInst);
	}
}
