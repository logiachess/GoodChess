#include "init.h"
#include "attacks.h"
#include "board.h"


Bitboard pawn_attacks[2][64];


void init_leaper_attacks()
{

	// loop 64 squares
	for (int square = 0; square < 64; ++square)
	{
		// pawn attacks
		pawn_attacks[WHITE][square] = mask_pawn_attacks(WHITE, square);
		pawn_attacks[BLACK][square] = mask_pawn_attacks(BLACK, square);
	}
}



void InitAll()
{
	init_leaper_attacks();
}


























