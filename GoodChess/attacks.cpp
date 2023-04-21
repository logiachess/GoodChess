#include "attacks.h"
#include "bitboard.h"


U64 mask_pawn_attacks(int side, int square)
{

	U64 attacks = 0ULL; // attacks bitboard
	U64 bitboard = 0ULL; // piece bitboard

	set_bit(bitboard, square); // set piece on board

	// white pawns
	if (!side)
	{
		if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
	}
	// black pawns
	else
	{
		if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
		if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
	}

	return attacks;
}



