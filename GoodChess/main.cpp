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

		print_bitboard(knight_attacks[square]);
		printf("\n bits:%d\n", bitscan_reverse(knight_attacks[square]));
	}
	getchar();




	return 0;
}




















