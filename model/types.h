#ifndef SALIS_TYPES_H
#define SALIS_TYPES_H

#include <limits.h>

#define SWORD_MAX (0xffffffff)
#define SBYTE_MAX (0xff)

#if USHRT_MAX == SWORD_MAX
	typedef unsigned short sword;
#elif UINT_MAX == SWORD_MAX
	typedef unsigned int sword;
#elif ULONG_MAX == SWORD_MAX
	typedef unsigned long sword;
#elif
	#error "Cannot define 32 bit unsigned int (sword)"
#endif

#if UCHAR_MAX == SBYTE_MAX
	typedef unsigned char sbyte;
#elif
	#error "Cannot define 8 bit unsigned int (sbyte)"
#endif

typedef int sbool;

#define SFALSE (0)
#define STRUE  (1)
#define SNULL  ((sword)-1)

#endif
