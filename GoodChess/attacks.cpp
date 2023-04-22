#include "attacks.h"
#include "bitboard.h"
#include "magic.h"



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




Bitboard mask_bishop_attacks(int square, Bitboard occupied)
{
	// result attacks bitboard
	Bitboard attacks = 0ULL;

	int rank, file;

	int target_rank = square / 8;
	int target_file = square % 8;

	// mask relevant bits
	for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; ++rank, ++file) {

		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & occupied) break;
	}
	for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; --rank, --file) {

		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & occupied) break;
	}
	for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; --rank, ++file) {

		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & occupied) break;
	}
	for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; ++rank, --file) {

		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & occupied) break;
	}

	// return relative attacks squares
	return attacks;
}




Bitboard mask_rook_attacks(int square, Bitboard occupied)
{
	// result attacks bitboard
	Bitboard attacks = 0ULL;

	int rank, file;

	int target_rank = square / 8;
	int target_file = square % 8;

	// mask relevant bits
	for (rank = target_rank + 1; rank <= 7;  ++rank) {

		attacks |= (1ULL << (rank * 8 + target_file));
		if ((1ULL << (rank * 8 + target_file)) & occupied) break;
	}
	for (rank = target_rank - 1; rank >= 0; --rank) {

		attacks |= (1ULL << (rank * 8 + target_file));
		if ((1ULL << (rank * 8 + target_file)) & occupied) break;
	}
	for (file = target_file + 1; file <= 7; ++file) {

		attacks |= (1ULL << (target_rank * 8 + file));
		if ((1ULL << (target_rank * 8 + file)) & occupied) break;
	}
	for (file = target_file - 1; file >= 0; --file) {

		attacks |= (1ULL << (target_rank * 8 + file));
		if ((1ULL << (target_rank * 8 + file)) & occupied) break;
	}

	// return relative attacks squares
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


Bitboard mask_rook_occupancy(int square)
{
	// result occupancy bitboard
	Bitboard occupancy = 0ULL;

	int rank, file;

	int target_rank = square / 8;
	int target_file = square % 8;

	// mask relevant bits
	for (rank = target_rank + 1;  rank <= 6; ++rank) {
		occupancy |= (1ULL << (rank * 8 + target_file));
	}
	for (rank = target_rank - 1;  rank >= 1; --rank) {
		occupancy |= (1ULL << (rank * 8 + target_file));
	}
	for (file = target_file + 1;  file <= 6; ++file) {
		occupancy |= (1ULL << (target_rank * 8 + file));
	}
	for (file = target_file - 1;  file >= 1; --file) {
		occupancy |= (1ULL << (target_rank * 8 + file));
	}

	// return relative occupancy squares
	return occupancy;
}

Bitboard rook_attacks[64][4096];
Bitboard bishop_attacks[64][512];
Bitboard bishop_masks[64];
Bitboard rook_masks[64];


void init_sliders_attacks(PieceType pt)
{
	U64 occupancy = 0ULL;
	set_bit(occupancy, b6);

	for (int square = 0; square < 64; ++square)
	{
		bishop_masks[square] = mask_bishop_occupancy(square);
		rook_masks[square] = mask_rook_occupancy(square);

		Bitboard attack_mask = (pt == BISHOP) ? bishop_masks[square] : rook_masks[square];
		int relevant_bit_count = count_bits(attack_mask);
		int occupancy_indices = (1 << relevant_bit_count);

		for (int index = 0; index < occupancy_indices; ++index)
		{
			if (pt == BISHOP)
			{
				Bitboard occupancy = set_occupancy(index, relevant_bit_count, attack_mask);

				int magic_index = (occupancy * bishop_magics[square]) >> (64 - bishop_relevant_occupancy[square]);

				bishop_attacks[square][magic_index] = mask_bishop_attacks(square, occupancy);
			}
			else
			{
				Bitboard occupancy = set_occupancy(index, relevant_bit_count, attack_mask);

				int magic_index = (occupancy * rook_magics[square]) >> (64 - rook_relevant_occupancy[square]);

				rook_attacks[square][magic_index] = mask_rook_attacks(square, occupancy);
			}
		}
	}
}



static inline Bitboard get_bishop_attacks(int square, Bitboard occupancy)
{
	occupancy &= bishop_masks[square];
	occupancy *= bishop_magics[square];
	occupancy >>= 64 - bishop_relevant_occupancy[square];
	return bishop_attacks[square][occupancy];
}


static inline Bitboard get_rook_attacks(int square, Bitboard occupancy)
{
	occupancy &= rook_masks[square];
	occupancy *= rook_magics[square];
	occupancy >>= 64 - rook_relevant_occupancy[square];
	return rook_attacks[square][occupancy];
}


static inline Bitboard get_queen_attacks(int square, Bitboard occupancy)
{
	Bitboard queen_attacks = 0ULL;
	Bitboard rook_occupancy = occupancy;
	Bitboard bishop_occupancy = occupancy;

	bishop_occupancy &= bishop_masks[square];
	bishop_occupancy *= bishop_magics[square];
	bishop_occupancy >>= 64 - bishop_relevant_occupancy[square];
	queen_attacks = bishop_attacks[square][bishop_occupancy];

	rook_occupancy &= rook_masks[square];
	rook_occupancy *= rook_magics[square];
	rook_occupancy >>= 64 - rook_relevant_occupancy[square];
	queen_attacks |= rook_attacks[square][rook_occupancy];

	return queen_attacks;
}

