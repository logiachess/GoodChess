#include "pvtable.h"
#include "movegen.h"
#include "board.h"
#include <cstdio>


HASHTABLE HashTable[1];


// Return move from posKey
static inline int ProbePvMove(const BOARD* pos, const HASHTABLE* table)
{

	int index = pos->posKey % table->numEntries;


	U64 test_key = pos->posKey ^ table->pTable[index].smp_data; // Test Key to verify position

	// Check for matching key

	if (table->pTable[index].smp_key == test_key)
	{
		return EXTRACT_MOVE(table->pTable[index].smp_data);
	}

	return 0;
}

void GetPvLineHelper(const int depth, BOARD*pos, const HASHTABLE* table, int* count)
{
	int move = ProbePvMove(pos, table);
	if (move != 0 && *count < depth)
	{
		copy_board(pos);
		if (MoveExists(pos, move))
		{
			pos->pvArray[(*count)++] = move;
			GetPvLineHelper(depth, pos, table, count);
			take_board(pos);
		}
		else
		{
			print_board(pos);
			printf("\n\n%s\n\n", Pr_move(move));
		}
	}
	return;
}


// Fill PvLine array
int GetPvLine(const int depth, BOARD* pos, const HASHTABLE* table)
{
	int count = 0;
	GetPvLineHelper(depth, pos, table, &count);

	return count;
}


// Function to initialize pvTable
void InitHashTable(HASHTABLE* table, const int MB)
{

	int HashSize = 0x100000 * MB;
	table->numEntries = HashSize / sizeof(HASHENTRY);
	table->numEntries -= 2;

	if (table->pTable != NULL)
	{
		free(table->pTable);
	}

	table->pTable = (HASHENTRY*)malloc(table->numEntries * sizeof(HASHENTRY));
	if (table->pTable == NULL)
	{
		printf("Hash Allocation Failed, trying %dMB...\n", MB / 2);
		InitHashTable(table, MB / 2);
	}
	else
	{
		ClearHashTable(table);
		printf("HashTable init complete with %d entries\n", table->numEntries);
	}
}

void ClearHashTable(HASHTABLE* table)
{

	HASHENTRY* tableEntry;

	for (tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; ++tableEntry)
	{
		tableEntry->age = 0;
		tableEntry->smp_data = 0ULL;
		tableEntry->smp_key = 0ULL;

	}
	table->currentAge = 0;
	table->newWrite = 0;
}


// Store move into pvTable
void StoreHashEntry(BOARD* pos, HASHTABLE* table, const int move, int score, const int flags, const int depth)
{

	int index = pos->posKey % table->numEntries;


	int replace = FALSE;

	if (table->pTable[index].smp_key == 0)
	{
		++table->newWrite;
		replace = TRUE;
	}
	else
	{
		if (table->pTable[index].age < table->currentAge)
		{
			replace = TRUE;
		}
		else if (EXTRACT_DEPTH(table->pTable[index].smp_data) <= depth)
		{
			replace = TRUE;
		}
	}

	if (replace == FALSE) return;

	if (score > VALUE_MATE_IN_MAX_PLY)
		score += pos->ply;
	else if (score < VALUE_MATED_IN_MAX_PLY)
		score -= pos->ply;

	// Create smp entry
	U64 smp_data = FOLD_DATA(score, depth, flags, move);

	table->pTable[index].age = table->currentAge;
	table->pTable[index].smp_data = smp_data;
	table->pTable[index].smp_key = pos->posKey ^ smp_data;
}


int ProbeHashEntry(BOARD* pos, HASHTABLE* table, int* move, int* score, int alpha, int beta, int depth) {

	int index = pos->posKey % table->numEntries;

	U64 test_key = pos->posKey ^ table->pTable[index].smp_data; // Test Key to verify position

	if (table->pTable[index].smp_key == test_key) {

		int smp_depth = EXTRACT_DEPTH(table->pTable[index].smp_data);
		int smp_move = EXTRACT_MOVE(table->pTable[index].smp_data);
		int smp_score = EXTRACT_SCORE(table->pTable[index].smp_data);
		int smp_flags = EXTRACT_FLAGS(table->pTable[index].smp_data);

		*move = smp_move;
		if (smp_depth >= depth) {
			++table->hit;

			*score = smp_score;
			if (*score > VALUE_MATE_IN_MAX_PLY) *score -= pos->ply;
			else if (*score < VALUE_MATED_IN_MAX_PLY) *score += pos->ply;

			switch (smp_flags) {

			case HFALPHA: if (*score <= alpha) {
				*score = alpha;
				return TRUE;
			}
						break;
			case HFBETA: if (*score >= beta) {
				*score = beta;
				return TRUE;
			}
					   break;
			case HFEXACT:
				return TRUE;
				break;
			default: break;
			}
		}
	}

	return FALSE;
}



