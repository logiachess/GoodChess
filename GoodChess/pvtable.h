#ifndef PVTABLE_H_INCLUDED
#define PVTABLE_INCLUDED


/* HEADERS */
#include "types.h"


/* DEFINITIONS */
typedef struct
{
	int age;

	U64 smp_data; // Data
	U64 smp_key; // Position key
} HASHENTRY;

typedef struct
{
	HASHENTRY* pTable;
	int numEntries;
	int newWrite;
	int overWrite;
	int hit;
	int cut;
	int currentAge;
} HASHTABLE;


/* MACROS */
#define EXTRACT_SCORE(x) ((x & 0xFFFF) - VALUE_INFINITE)
#define EXTRACT_DEPTH(x) ((x>>16) & 0x3F)
#define EXTRACT_FLAGS(x) ((x>>23) & 0x3)
#define EXTRACT_MOVE(x) ((int)(x>>25))

#define FOLD_DATA(sc, de, fl, mv) ( (sc + VALUE_INFINITE) | (de << 16) | (fl<<23) | ((U64)mv << 25)) // Create data


/* FUNCTIONS */
void InitHashTable(HASHTABLE* table, const int MB);

void ClearHashTable(HASHTABLE* table);




#endif