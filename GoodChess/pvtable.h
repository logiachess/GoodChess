#ifndef PVTABLE_H_INCLUDED
#define PVTABLE_INCLUDED


/* HEADERS */
#include "types.h"


/* DEFINITIONS */
extern HASHTABLE HashTable[1];


/* MACROS */
#define EXTRACT_SCORE(x) ((x & 0xFFFF) - VALUE_INFINITE)
#define EXTRACT_DEPTH(x) ((x>>16) & 0x3F)
#define EXTRACT_FLAGS(x) ((x>>23) & 0x3)
#define EXTRACT_MOVE(x) ((int)(x>>25))

#define FOLD_DATA(sc, de, fl, mv) ( (sc + VALUE_INFINITE) | (de << 16) | (fl<<23) | ((U64)mv << 25)) // Create data


/* FUNCTIONS */
void InitHashTable(HASHTABLE* table, const int MB);
int GetPvLine(const int depth, BOARD* pos, const HASHTABLE* table);
void ClearHashTable(HASHTABLE* table);
void StoreHashEntry(BOARD* pos, HASHTABLE* table, const int move, int score, const int flags, const int depth);




#endif