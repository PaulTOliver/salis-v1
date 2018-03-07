#ifndef SALIS_H
#define SALIS_H

#include <stdio.h>
#include <types.h>
#include <instset.h>
#include <memory.h>
#include <process.h>
#include <evolver.h>

void  s_init     (sword order);
void  s_quit     (void);
void  s_load     (const char *fileName);
void  s_save     (const char *fileName);

sbool s_isInit   (void);
sword s_getCycle (void);
sword s_getEpoch (void);

void  s_cycle    (void);

#endif
