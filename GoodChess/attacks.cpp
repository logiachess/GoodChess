#include "attacks.h"
#include "bitboard.h"


Bitboard mask_pawn_attacks(int side, int square)
{

	Bitboard attacks = 0ULL; // attacks bitboard
	Bitboard bitboard = 0ULL; // piece bitboard

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


Bitboard mask_knight_attacks(int square)
{

	Bitboard attacks = 0ULL; // attacks bitboard
	Bitboard bitboard = 0ULL; // piece bitboard

	// set piece on board
	set_bit(bitboard, square);

	// generate knight attacks
	if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
	if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
	if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
	if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
	if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
	if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
	if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
	if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

	// return attack map
	return attacks;
}


Bitboard mask_king_attacks(int square)
{
	// result attacks bitboard
	Bitboard attacks = 0ULL;

	// piece bitboard
	Bitboard bitboard = 0ULL;

	// set piece on board
	set_bit(bitboard, square);

	// generate king attacks
	if (bitboard >> 8) attacks |= (bitboard >> 8);
	if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
	if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
	if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
	if (bitboard << 8) attacks |= (bitboard << 8);
	if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
	if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
	if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);

	// return attack map
	return attacks;
}




