#include <assert.h>
#include <stdlib.h>
#include "salis.h"

static sbool g_isInit;
static sword g_cycle;
static sword g_epoch;

void
s_init(sword order)
{
	assert(!g_isInit);
	sm_init(order);
	se_init();
	sp_init();
	g_isInit = STRUE;
}

void
s_quit(void)
{
	assert(g_isInit);
	sp_quit();
	se_quit();
	sm_quit();
	g_isInit = SFALSE;
	g_cycle  = 0;
	g_epoch  = 0;
}

void
s_load(const char *fileName)
{
	FILE *file;
	assert(!g_isInit);
	assert(fileName);
	file = fopen(fileName, "rb");
	assert(file);
	fread(&g_isInit, sizeof(sbool), 1, file);
	fread(&g_cycle,  sizeof(sword), 1, file);
	fread(&g_epoch,  sizeof(sword), 1, file);
	sm_load(file);
	se_load(file);
	sp_load(file);
	fclose(file);
}

void
s_save(const char *fileName)
{
	FILE *file;
	assert(g_isInit);
	assert(fileName);
	file = fopen(fileName, "wb");
	assert(file);
	fwrite(&g_isInit, sizeof(sbool), 1, file);
	fwrite(&g_cycle,  sizeof(sword), 1, file);
	fwrite(&g_epoch,  sizeof(sword), 1, file);
	sm_save(file);
	se_save(file);
	sp_save(file);
	fclose(file);
}

sbool
s_isInit(void)
{
	return g_isInit;
}

sword
s_getCycle(void)
{
	return g_cycle;
}

sword
s_getEpoch(void)
{
	return g_epoch;
}

void
s_cycle(void)
{
	assert(g_isInit);
	assert(sm_isInit());
	assert(sp_isInit());
	assert(se_isInit());
	g_cycle++;

	if (!g_cycle) {
		g_epoch++;
	}

	se_cycle();
	sp_cycle();
}
