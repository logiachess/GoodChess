#include "pvtable.h"
#include "board.h"
#include <cstdio>


// Function to initialize pvTable
void InitHashTable(HASHTABLE* table, const int MB) {

	int HashSize = 0x100000 * MB;
	table->numEntries = HashSize / sizeof(HASHENTRY);
	table->numEntries -= 2;

	if (table->pTable != NULL) {
		free(table->pTable);
	}

	table->pTable = (HASHENTRY*)malloc(table->numEntries * sizeof(HASHENTRY));
	if (table->pTable == NULL) {
		printf("Hash Allocation Failed, trying %dMB...\n", MB / 2);
		InitHashTable(table, MB / 2);
	}
	else {
		ClearHashTable(table);
		printf("HashTable init complete with %d entries\n", table->numEntries);
	}
}

void ClearHashTable(HASHTABLE* table) {

	HASHENTRY* tableEntry;

	for (tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; ++tableEntry) {
		tableEntry->age = 0;
		tableEntry->smp_data = 0ULL;
		tableEntry->smp_key = 0ULL;

	}
	table->currentAge = 0;
	table->newWrite = 0;
}


// Store move into pvTable
void StoreHashEntry(BOARD* pos, HASHTABLE* table, const int move, int score, const int flags, const int depth) {

	int index = pos->posKey % table->numEntries;


	int replace = FALSE;

	if (table->pTable[index].smp_key == 0) {
		++table->newWrite;
		replace = TRUE;
	}
	else {
		if (table->pTable[index].age < table->currentAge) {
			replace = TRUE;
		}
		else if (EXTRACT_DEPTH(table->pTable[index].smp_data) <= depth) {
			replace = TRUE;
		}
	}

	if (replace == FALSE) return;

	if (score > VALUE_MATED_IN_MAX_PLY)
		score -= pos->ply;
	else if (score < VALUE_MATED_IN_MAX_PLY)
		score += pos->ply;

	// Create smp entry
	U64 smp_data = FOLD_DATA(score, depth, flags, move);

	table->pTable[index].age = table->currentAge;
	table->pTable[index].smp_data = smp_data;
	table->pTable[index].smp_key = pos->posKey ^ smp_data;
}




