#ifndef SALIS_PROCESS_H
#define SALIS_PROCESS_H

#define SPROC_REG_COUNT   4
#define SPROC_STACK_SIZE  8
#define SPROC_ELEM_COUNT (6 + SPROC_REG_COUNT + SPROC_STACK_SIZE)

typedef struct {
	sword mb1a;
	sword mb1s;
	sword mb2a;
	sword mb2s;

	sword ip;
	sword sp;

	sword regs  [SPROC_REG_COUNT];
	sword stack [SPROC_STACK_SIZE];
} SProc;

void  sp_init     (void);
void  sp_quit     (void);
void  sp_load     (FILE *file);
void  sp_save     (FILE *file);

sbool sp_isInit   (void);
sword sp_getCount (void);
sword sp_getCap   (void);
sword sp_getFirst (void);
sword sp_getLast  (void);

sbool sp_isFree   (sword pidx);
SProc sp_getProc  (sword pidx);
void  sp_setProc  (sword pidx, SProc proc);

void  sp_create   (sword addr, sword size);
void  sp_kill     (void);
void  sp_cycle    (void);

#endif
