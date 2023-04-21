#include "init.h"
#include "attacks.h"
#include "board.h"


Bitboard pawn_attacks[2][64];
Bitboard knight_attacks[64];
Bitboard king_attacks[64];


void init_leaper_attacks()
{

	// loop 64 squares
	for (int square = 0; square < 64; ++square)
	{
		// pawn attacks
		pawn_attacks[WHITE][square] = mask_pawn_attacks(WHITE, square);
		pawn_attacks[BLACK][square] = mask_pawn_attacks(BLACK, square);

		// knight attacks
		knight_attacks[square] = mask_knight_attacks(square);

		// king attacks
		king_attacks[square] = mask_king_attacks(square);
	}
}


void init_slider_attacks()
{

	// loop 64 squares
	for (int square = 0; square < 64; ++square)
	{
		// rook attacks

		// bishop attacks
		
		// queen attacks
	}
}



void InitAll()
{
	init_leaper_attacks();
	init_slider_attacks;
}


























