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
		if ((bitboard >> 7) & ~FileABB) attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & ~FileHBB) attacks |= (bitboard >> 9);
	}
	// black pawns
	else
	{
		if ((bitboard << 7) & ~FileHBB) attacks |= (bitboard << 7);
		if ((bitboard << 9) & ~FileABB) attacks |= (bitboard << 9);
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
	if ((bitboard >> 17) & ~FileHBB) attacks |= (bitboard >> 17);
	if ((bitboard >> 15) & ~FileABB) attacks |= (bitboard >> 15);
	if ((bitboard >> 10) & ~FileHGBB) attacks |= (bitboard >> 10);
	if ((bitboard >> 6) & ~FileABBB) attacks |= (bitboard >> 6);
	if ((bitboard << 17) & ~FileABB) attacks |= (bitboard << 17);
	if ((bitboard << 15) & ~FileHBB) attacks |= (bitboard << 15);
	if ((bitboard << 10) & ~FileABBB) attacks |= (bitboard << 10);
	if ((bitboard << 6) & ~FileHGBB) attacks |= (bitboard << 6);

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
	if ((bitboard >> 9) & ~FileHBB) attacks |= (bitboard >> 9);
	if ((bitboard >> 7) & ~FileABB) attacks |= (bitboard >> 7);
	if ((bitboard >> 1) & ~FileHBB) attacks |= (bitboard >> 1);
	if (bitboard << 8) attacks |= (bitboard << 8);
	if ((bitboard << 9) & ~FileABB) attacks |= (bitboard << 9);
	if ((bitboard << 7) & ~FileHBB) attacks |= (bitboard << 7);
	if ((bitboard << 1) & ~FileABB) attacks |= (bitboard << 1);

	// return attack map
	return attacks;
}


Bitboard mask_bishop_occupancy(int square)
{
	// result occupancy bitboard
	Bitboard occupancy = 0ULL;

	int rank, file;

	int target_rank = square / 8;
	int target_file = square % 8;

	// mask relevant bits
	for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; ++rank, ++file) {
		occupancy |= (1ULL << (rank * 8 + file));
	}
	for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; --rank, --file) {
		occupancy |= (1ULL << (rank * 8 + file));
	}
	for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; --rank, ++file) {
		occupancy |= (1ULL << (rank * 8 + file));
	}
	for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; ++rank, --file) {
		occupancy |= (1ULL << (rank * 8 + file));
	}

	// return relative occupancy squares
	return occupancy;
}




