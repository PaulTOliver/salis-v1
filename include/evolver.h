#ifndef SALIS_EVOLVER_H
#define SALIS_EVOLVER_H

void  se_init           (void);
void  se_quit           (void);
void  se_load           (FILE *file);
void  se_save           (FILE *file);

sbool se_isInit         (void);
sword se_getLastAddress (void);
sbyte se_getLastInst    (void);
sword se_getState       (sword eidx);
void  se_setState       (sword eidx, sword state);

void  se_cycle          (void);

#endif
