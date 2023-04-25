#ifndef MOVEGEN_H_INCLUDED
#define MOVEGEN_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "board.h"
#include "bitboard.h"
#include "magic.h"
#include "attacks.h"	


/* DEFINITIONS */


/* MACROS */


/* FUNCTIONS */
static inline void add_move(Moves_list* moves_list, int move)
{
	moves_list->moves[moves_list->count] = move;
	++moves_list->count;
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


static inline int is_square_attacked(int square, int side)
{
	if (side == WHITE) {

		if ((get_rook_attacks(square, occupancies[BOTH])) & (bitboards[WR])) return TRUE;
		if ((get_queen_attacks(square, occupancies[BOTH])) & (bitboards[WQ])) return TRUE;

		if (pawn_attacks[BLACK][square] & bitboards[WP]) return TRUE;

		if (knight_attacks[square] & (bitboards[WN])) return TRUE;
		if ((get_bishop_attacks(square, occupancies[BOTH])) & (bitboards[WB])) return TRUE;
		if (king_attacks[square] & (bitboards[WK])) return TRUE;
	}
	else {
		if ((get_rook_attacks(square, occupancies[BOTH])) & (bitboards[BR])) return TRUE;
		if ((get_queen_attacks(square, occupancies[BOTH])) & (bitboards[BQ])) return TRUE;

		if ((side == BLACK) && (pawn_attacks[WHITE][square] & bitboards[BP])) return TRUE;

		if (knight_attacks[square] & (bitboards[BN])) return TRUE;
		if ((get_bishop_attacks(square, occupancies[BOTH])) & (bitboards[BB])) return TRUE;
		if (king_attacks[square] & (bitboards[BK])) return TRUE;
	}

	return FALSE;
}



// static inline
static inline void generate_moves(Moves_list *list)
{
	list->count = 0;

	
	int from, to;

	Bitboard bitboard, attacks;

	Piece start_piece = (side == WHITE) ? WP : BP;
	Piece end_piece = (side == WHITE) ? WK : BK;

	for (Piece piece = start_piece; piece <= end_piece; ++piece)
	{
		bitboard = bitboards[piece];

		if (side == WHITE)
		{
			switch (piece)
			{
			case WP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);
					to = from - 8;

					if (!(to < a8) && !get_bit(occupancies[BOTH], to))
					{
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							printf("pawn promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, WQ, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WR, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WB, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WN, 0, 0, 0, 0));
						}
						else
						{
							// one square ahead 

							printf("pawn push: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
							if ((h2 >= from && from >= a2) && !get_bit(occupancies[BOTH], to - 8))
							{
								// two square move
								printf("double pawn push: %s%s\n", sq_to_coord[from], sq_to_coord[to - 8]);
								add_move(list, encode_move(from, to-8, piece, NO_PIECE, 0, 1, 0, 0));
							}
						}
					}
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[BLACK];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							printf("pawn x promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, WQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							printf("pawn x: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[side][from] & (1ULL << enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							printf("pawn x enpassant: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0));
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case WK:
				if (castle & WKCA)
				{
					// check empty
					if (!get_bit(occupancies[BOTH], f1) && !get_bit(occupancies[BOTH], g1))
					{
						// check safety
						if (!is_square_attacked(f1, BLACK) && !is_square_attacked(e1, BLACK))
						{
							printf("Castle: O-O\n");
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 1));
						}

					}
				}
				if (castle & WQCA)
				{
					// check empty
					if (!get_bit(occupancies[BOTH], d1) && !get_bit(occupancies[BOTH], c1) && !get_bit(occupancies[BOTH], b1))
					{
						// check safety
						if (!is_square_attacked(d1, BLACK) && !is_square_attacked(e1, BLACK))
						{
							printf("Castle: O-O-O\n");
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 1));
						}

					}
				}
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("kingx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							printf("king: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WN:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (knight_attacks[from]) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("knightx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							printf("knight: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WB:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_bishop_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("bishopx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							printf("bishop:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WR:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_rook_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("rookx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							printf("rook:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WQ:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_queen_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("queenx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							printf("queen:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			}
		}
		else
		{
			switch (piece)
			{
			case BP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);
					to = from + 8;

					if (!(to > h1) && !get_bit(occupancies[BOTH], to))
					{
						if (h2 >= from && from >= a2)
						{
							// 4 promotions
							printf("pawn promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, BQ, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BR, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BB, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BN, 0, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							printf("pawn push: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
							if ((h7 >= from && from >= a7) && !get_bit(occupancies[BOTH], to + 8))
							{
								// two square move
								printf("double pawn push: %s%s\n", sq_to_coord[from], sq_to_coord[to + 8]);
								add_move(list, encode_move(from, to + 8, piece, NO_PIECE, 0, 1, 0, 0));
							}
						}
					}
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[WHITE];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h2 >= from && from >= a2)
						{
							// 4 promotions
							printf("pawn x promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, BQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							printf("pawn x: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[side][from] & (1ULL << enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							printf("pawn x enpassant: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case BK:
				if (castle & BKCA)
				{
					// check empty
					if (!get_bit(occupancies[BOTH], f8) && !get_bit(occupancies[BOTH], g8))
					{
						// check safety
						if (!is_square_attacked(f8, WHITE) && !is_square_attacked(e8, WHITE))
						{
							printf("Castle: O-O\n");
						}

					}
				}
				if (castle & BQCA)
				{
					// check empty
					if (!get_bit(occupancies[BOTH], d8) && !get_bit(occupancies[BOTH], c8) && !get_bit(occupancies[BOTH], b8))
					{
						// check safety
						if (!is_square_attacked(d8, WHITE) && !is_square_attacked(e8, WHITE))
						{
							printf("Castle: O-O-O\n");
						}

					}
				}
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("kingx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("king: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BN:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = knight_attacks[from] & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (!get_bit(occupancies[WHITE], to))
						{
							printf("knight: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("knight x : %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}
					pop_bit(bitboard, from);
				}
				break;
			case BB:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_bishop_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("bishopx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("bishop:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BR:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_rook_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("rookx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("rook:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BQ:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_queen_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("queenx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("queen:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			}
		}
	}
}


// static inline
static inline void generate_captures()
{
	int from, to;

	Bitboard bitboard, attacks;

	Piece start_piece = (side == WHITE) ? WP : BP;
	Piece end_piece = (side == WHITE) ? WK : BK;

	for (Piece piece = start_piece; piece <= end_piece; ++piece)
	{
		bitboard = bitboards[piece];

		if (side == WHITE)
		{
			switch (piece)
			{
			case WP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);

					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[BLACK];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							printf("pawn x promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							// one square ahead move
							printf("pawn x: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[side][from] & (1ULL << enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							printf("pawn x enpassant: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case WK:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("kingx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WN:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (knight_attacks[from]) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("knightx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WB:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_bishop_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("bishopx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WR:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_rook_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("rookx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case WQ:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_queen_attacks(from, occupancies[BOTH])) & (~occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[BLACK], to))
						{
							printf("queenx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			}
		}
		else
		{
			switch (piece)
			{
			case BP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);

					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[WHITE];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h2 >= from && from >= a2)
						{
							// 4 promotions
							printf("pawn x promotion: %s%sq\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sr\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sb\n", sq_to_coord[from], sq_to_coord[to]);
							printf("pawn x promotion: %s%sn\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							// one square ahead move
							printf("pawn x: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[side][from] & (1ULL << enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							printf("pawn x enpassant: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case BK:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("kingx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}
						else
						{
							printf("king: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BN:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = knight_attacks[from] & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("knightx: %s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}
					pop_bit(bitboard, from);
				}
				break;
			case BB:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_bishop_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("bishopx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BR:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_rook_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("rookx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			case BQ:
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (get_queen_attacks(from, occupancies[BOTH])) & (~occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(occupancies[WHITE], to))
						{
							printf("queenx:%s%s\n", sq_to_coord[from], sq_to_coord[to]);
						}

						pop_bit(attacks, to);
					}

					pop_bit(bitboard, from);
				}
				break;
			}
		}
	}
}





#endif