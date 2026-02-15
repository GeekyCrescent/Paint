/* Paint for DOS - Type Definitions
 * Copyright (C) 2024
 */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* _TYPES_H_ */
