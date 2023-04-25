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
							add_move(list, encode_move(from, to, piece, WQ, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WR, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WB, 0, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WN, 0, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
							if ((h2 >= from && from >= a2) && !get_bit(occupancies[BOTH], to - 8))
							{
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
							add_move(list, encode_move(from, to, piece, WQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
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
							add_move(list, encode_move(from, to, piece, BQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 1));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 1));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
						}
						else
						{
							add_move(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0));
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
static inline void generate_captures(Moves_list *list)
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
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[BLACK];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							add_move(list, encode_move(from, to, piece, WQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, WN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
					
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[side][from] & occupancies[WHITE];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h2 >= from && from >= a2)
						{
							add_move(list, encode_move(from, to, piece, BQ, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BR, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BB, 1, 0, 0, 0));
							add_move(list, encode_move(from, to, piece, BN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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
							add_move(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0));
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

static inline int make_move(int move, int move_flag)
{
	if (move_flag == ALL_MOVE)
	{
		Board_copy copy = copy_board();

		int from = get_move_from(move);
		int to = get_move_to(move);
		int piece = get_move_piece(move);
		int promoted = get_move_promotion(move);
		int capture = get_move_capture(move);
		int double_PP = get_move_double(move);
		int enpass = get_move_enpas(move);
		int castling = get_move_castle(move);

		pop_bit(bitboards[piece], from);
		set_bit(bitboards[piece], to);
	}
	else
	{
		if (get_move_capture(move)) make_move(move, ALL_MOVE);
		else return 0;
	}
	return 0;
}






#endif