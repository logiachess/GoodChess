#include <stdio.h>
#include "init.h"
#include "bitboard.h"
#include "board.h"
#include "attacks.h"


int main()
{
	InitAll();

	// loop 64 squares
	for (int square = 0; square < 64; ++square)
	{

		print_bitboard(pawn_attacks[BLACK][square]);
	}
	getchar();
	return 0;
}




















