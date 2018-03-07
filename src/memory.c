#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "instset.h"
#include "memory.h"

#define MEM_BLOCK_START_FLAG (0x80)
#define ALLOCATED_FLAG       (0x40)
#define INSTRUCTION_MASK     (0x3f)

static sbool  g_isInit;
static sword  g_order;
static sword  g_size;
static sword  g_memBlockCount;
static sword  g_allocated;
static sword  g_cap;
static sbyte *g_data;

void
sm_init(sword order)
{
	assert(!g_isInit);
	assert(INSTRUCTION_MASK == SINST_COUNT - 1);
	g_isInit = STRUE;
	g_order  = order;
	g_size   = 1 << g_order;
	g_cap    = g_size / 2;
	g_data   = calloc(g_size, sizeof(sbyte));
	assert(g_data);
}

void
sm_quit(void)
{
	assert(g_isInit);
	free(g_data);
	g_isInit        = SFALSE;
	g_order         = 0;
	g_size          = 0;
	g_memBlockCount = 0;
	g_allocated     = 0;
	g_cap           = 0;
	g_data          = NULL;
}

void
sm_load(FILE *file)
{
	assert(!g_isInit);
	assert(file);
	fread(&g_isInit,        sizeof(sbool), 1, file);
	fread(&g_order,         sizeof(sword), 1, file);
	fread(&g_size,          sizeof(sword), 1, file);
	fread(&g_memBlockCount, sizeof(sword), 1, file);
	fread(&g_allocated,     sizeof(sword), 1, file);
	fread(&g_cap,           sizeof(sword), 1, file);
	g_data = calloc(g_size, sizeof(sbyte));
	assert(g_data);
	fread(g_data, sizeof(sbyte), g_size, file);
}

void
sm_save(FILE *file)
{
	assert(g_isInit);
	assert(file);
	fwrite(&g_isInit,        sizeof(sbool), 1,      file);
	fwrite(&g_order,         sizeof(sword), 1,      file);
	fwrite(&g_size,          sizeof(sword), 1,      file);
	fwrite(&g_memBlockCount, sizeof(sword), 1,      file);
	fwrite(&g_allocated,     sizeof(sword), 1,      file);
	fwrite(&g_cap,           sizeof(sword), 1,      file);
	fwrite(g_data,           sizeof(sbyte), g_size, file);
}

sbool
sm_isInit(void)
{
	return g_isInit;
}

sword
sm_getOrder(void)
{
	return g_order;
}

sword
sm_getSize(void)
{
	return g_size;
}

sword
sm_getMemBlockCount(void)
{
	return g_memBlockCount;
}

sword
sm_getAllocated(void)
{
	return g_allocated;
}

sword
sm_getCap(void)
{
	return g_cap;
}

sbool
sm_isOverCap(void)
{
	assert(g_isInit);
	return g_allocated > g_cap;
}

sbool
sm_isValidAt(sword addr)
{
	assert(g_isInit);
	return addr < g_size;
}

sbool
sm_isMemBlockStartAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	return !!(g_data[addr] & MEM_BLOCK_START_FLAG);
}

sbool
sm_isAllocatedAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	return !!(g_data[addr] & ALLOCATED_FLAG);
}

void
sm_setMemBlockStartAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	assert(!sm_isMemBlockStartAt(addr));
	g_data[addr] ^= MEM_BLOCK_START_FLAG;
	g_memBlockCount++;
	assert(sm_isMemBlockStartAt(addr));
	assert(g_memBlockCount);
	assert(g_memBlockCount <= g_size);
}

void
sm_unsetMemBlockStartAt(sword addr)
{
	assert(g_isInit);
	assert(g_memBlockCount);
	assert(sm_isValidAt(addr));
	assert(sm_isMemBlockStartAt(addr));
	g_data[addr] ^= MEM_BLOCK_START_FLAG;
	g_memBlockCount--;
	assert(!sm_isMemBlockStartAt(addr));
	assert(g_memBlockCount <= g_size);
}

void
sm_allocateAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	assert(!sm_isAllocatedAt(addr));
	g_data[addr] ^= ALLOCATED_FLAG;
	g_allocated++;
	assert(sm_isAllocatedAt(addr));
	assert(g_allocated);
	assert(g_allocated <= g_size);
}

void
sm_freeAt(sword addr)
{
	assert(g_isInit);
	assert(g_allocated);
	assert(sm_isValidAt(addr));
	assert(sm_isAllocatedAt(addr));
	g_data[addr] ^= ALLOCATED_FLAG;
	g_allocated--;
	assert(!sm_isAllocatedAt(addr));
	assert(g_allocated <= g_size);
}

sbyte
sm_getInstAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	return g_data[addr] & INSTRUCTION_MASK;
}

void
sm_setInstAt(sword addr, sbyte inst)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	assert(si_isInst(inst));
	g_data[addr] &= (MEM_BLOCK_START_FLAG | ALLOCATED_FLAG);
	g_data[addr] |= inst;
}

sbyte
sm_getByteAt(sword addr)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	return g_data[addr];
}

void
sm_setByteAt(sword addr, sbyte byte)
{
	assert(g_isInit);
	assert(sm_isValidAt(addr));
	g_data[addr] = byte;
}
