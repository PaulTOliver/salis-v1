#ifndef SALIS_MEMORY_H
#define SALIS_MEMORY_H

void  sm_init                 (sword order);
void  sm_quit                 (void);
void  sm_load                 (FILE *file);
void  sm_save                 (FILE *file);

sbool sm_isInit               (void);
sword sm_getOrder             (void);
sword sm_getSize              (void);
sword sm_getMemBlockCount     (void);
sword sm_getAllocated         (void);
sword sm_getCap               (void);

sbool sm_isOverCap            (void);
sbool sm_isValidAt            (sword addr);
sbool sm_isMemBlockStartAt    (sword addr);
sbool sm_isAllocatedAt        (sword addr);

void  sm_setMemBlockStartAt   (sword addr);
void  sm_unsetMemBlockStartAt (sword addr);
void  sm_allocateAt           (sword addr);
void  sm_freeAt               (sword addr);
sbyte sm_getInstAt            (sword addr);
void  sm_setInstAt            (sword addr, sbyte inst);
sbyte sm_getByteAt            (sword addr);
void  sm_setByteAt            (sword addr, sbyte byte);

#endif
