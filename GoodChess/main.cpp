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

		print_bitboard(pawn_attacks[WHITE][square]);
		//printf("\n bits:%d\n", bitscan_reverse(knight_attacks[square]));
		if (square % 4 == 3) printf("\n");
	}
	printf("},{");
	for (int square = 0; square < 64; ++square)
	{

		print_bitboard(pawn_attacks[BLACK][square]);
		//printf("\n bits:%d\n", bitscan_reverse(knight_attacks[square]));
		if (square % 4 == 3) printf("\n");
	}

	//print_bitboard(mask_bishop_attacks(e4));


	getchar();



	return 0;
}




















