#pragma once

#include <stdbool.h>
#include <stdint.h>

#define NAMEDATALEN 64

typedef char NameData[NAMEDATALEN];
typedef unsigned int Oid;
typedef float float4;
typedef int32_t int32;
typedef uint32_t TransactionId;
typedef int16_t int16;

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint16 LocationIndex;
typedef struct
{
	uint32		xlogid;			/* high bits */
	uint32		xrecoff;		/* low bits */
} PageXLogRecPtr;
typedef struct ItemIdData
{
	unsigned	lp_off:15,		/* offset to tuple (from start of page) */
				lp_flags:2,		/* state of line pointer, see below */
				lp_len:15;		/* byte length of tuple */
} ItemIdData;