#ifndef MOVEGEN_H_INCLUDED
#define MOVEGEN_H_INCLUDED


/* HEADERS */
#include <cstring>
#include "types.h"
#include "board.h"
#include "bitboard.h"
#include "magic.h"
#include "attacks.h"
#include "search.h" // killer, heuristic


/* DEFINITIONS */
const int castling_rights[64] = {
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

// MVV LVA [attacker][victim]
static int mvv_lva[6][5] = {
	105, 205, 305, 405, 505,
	104, 204, 304, 404, 504,
	103, 203, 303, 403, 503,
	102, 202, 302, 402, 502,
	101, 201, 301, 401, 501,
	100, 200, 300, 400, 500
};


/* MACROS */


/* FUNCTIONS */
static inline int captured_piecetype(int move, BOARD*pos)
{
	int start_piece, end_piece;

	// pick up side to move
	if (pos->side == WHITE) { start_piece = BP; end_piece = BR; }
	else { start_piece = WP;  end_piece = WR; }

	// loop over bitboards opposite to the current side to move
	for (int bb_piece = start_piece; bb_piece <= end_piece;)
	{
		++bb_piece;
		// if there's a piece on the target square
		if (get_bit(pos->bitboards[bb_piece], get_move_to(move)))
		{
			// remove it from corresponding bitboard
			start_piece = bb_piece;
			break;
		}
	}
	return (start_piece % 6);
}

static inline void add_quiet_promotion(Moves_list* moves_list, int move, BOARD*pos)
{
	moves_list->moves[moves_list->count].move = move;
	if (pos->killer_moves[0][pos->ply] == move)
	{
		moves_list->moves[moves_list->count].score = 1'009'000;
	}
	else if (pos->killer_moves[1][pos->ply] == move)
	{
		moves_list->moves[moves_list->count].score = 1'008'000;
	}
	else
	{
		moves_list->moves[moves_list->count].score = 1'000'000;
	}
	++moves_list->count;
}

static inline void add_capture_promotion(Moves_list* moves_list, int move)
{
	moves_list->moves[moves_list->count].move = move;
	moves_list->moves[moves_list->count].score = 1'020'000;
	++moves_list->count;
}


static inline void add_quiet(Moves_list* moves_list, int move, BOARD*pos)
{
	moves_list->moves[moves_list->count].move = move;
	if (pos->killer_moves[0][pos->ply] == move)
	{
		moves_list->moves[moves_list->count].score = 9'000;
	}
	else if (pos->killer_moves[1][pos->ply] == move)
	{
		moves_list->moves[moves_list->count].score = 8'000;
	}
	else
	{
		moves_list->moves[moves_list->count].score = pos->history_heuristic[get_move_piece(move)][get_move_to(move)];
	}

	++moves_list->count;
}

static inline void add_capture(Moves_list* moves_list, int move, BOARD*pos)
{
	moves_list->moves[moves_list->count].move = move;
	moves_list->moves[moves_list->count].score = mvv_lva[get_move_piece(move) % 6][captured_piecetype(move, pos) % 6] + 10'000;
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


static inline int is_square_attacked(int square, int side, BOARD*pos)
{
	if (side == WHITE) {

		if ((get_rook_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[WR])) return TRUE;
		if ((get_queen_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[WQ])) return TRUE;

		if (pawn_attacks[BLACK][square] & pos->bitboards[WP]) return TRUE;

		if (knight_attacks[square] & (pos->bitboards[WN])) return TRUE;
		if ((get_bishop_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[WB])) return TRUE;
		if (king_attacks[square] & (pos->bitboards[WK])) return TRUE;
	}
	else {
		if ((get_rook_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[BR])) return TRUE;
		if ((get_queen_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[BQ])) return TRUE;

		if (pawn_attacks[WHITE][square] & pos->bitboards[BP]) return TRUE;

		if (knight_attacks[square] & (pos->bitboards[BN])) return TRUE;
		if ((get_bishop_attacks(square, pos->occupancies[BOTH])) & (pos->bitboards[BB])) return TRUE;
		if (king_attacks[square] & (pos->bitboards[BK])) return TRUE;
	}

	return FALSE;
}

// static inline
static inline void generate_moves(Moves_list *list, BOARD*pos)
{
	list->count = 0;

	
	int from, to;

	Bitboard bitboard, attacks;

	Piece start_piece = (pos->side == WHITE) ? WP : BP;
	Piece end_piece = (pos->side == WHITE) ? WK : BK;

	for (Piece piece = start_piece; piece <= end_piece; ++piece)
	{
		bitboard = pos->bitboards[piece];

		if (pos->side == WHITE)
		{
			switch (piece)
			{
			case WP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);
					to = from - 8;

					if (!(to < a8) && !get_bit(pos->occupancies[BOTH], to))
					{
						if (h7 >= from && from >= a7)
						{
							add_quiet_promotion(list, encode_move(from, to, piece, WQ, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, WR, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, WB, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, WN, 0, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
							if ((h2 >= from && from >= a2) && !get_bit(pos->occupancies[BOTH], to - 8))
							{
								add_quiet(list, encode_move(from, to-8, piece, NO_PIECE, 0, 1, 0, 0), pos);
							}
						}
					}
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[WHITE][from] & pos->occupancies[BLACK];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							add_capture_promotion(list, encode_move(from, to, piece, WQ, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WR, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WB, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (pos->enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[WHITE][from] & (1ULL << pos->enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0), pos);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case WK:
				if (pos->castle & WKCA)
				{
					// check empty
					if (!get_bit(pos->occupancies[BOTH], f1) && !get_bit(pos->occupancies[BOTH], g1))
					{
						// check safety
						if (!is_square_attacked(f1, BLACK, pos) && !is_square_attacked(e1, BLACK, pos))
						{
							add_quiet(list, encode_move(e1, g1, piece, NO_PIECE, 0, 0, 0, 1), pos);
						}

					}
				}
				if (pos->castle & WQCA)
				{
					// check empty
					if (!get_bit(pos->occupancies[BOTH], d1) && !get_bit(pos->occupancies[BOTH], c1) && !get_bit(pos->occupancies[BOTH], b1))
					{
						// check safety
						if (!is_square_attacked(d1, BLACK, pos) && !is_square_attacked(e1, BLACK, pos))
						{
							add_quiet(list, encode_move(e1, c1, piece, NO_PIECE, 0, 0, 0, 1), pos);
						}

					}
				}
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (knight_attacks[from]) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_bishop_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_rook_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_queen_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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

					if (!(to > h1) && !get_bit(pos->occupancies[BOTH], to))
					{
						if (h2 >= from && from >= a2)
						{
							add_quiet_promotion(list, encode_move(from, to, piece, BQ, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, BR, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, BB, 0, 0, 0, 0), pos);
							add_quiet_promotion(list, encode_move(from, to, piece, BN, 0, 0, 0, 0), pos);
						}
						else
						{
							// one square ahead move
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
							if ((h7 >= from && from >= a7) && !get_bit(pos->occupancies[BOTH], to + 8))
							{
								add_quiet(list, encode_move(from, to + 8, piece, NO_PIECE, 0, 1, 0, 0), pos);
							}
						}
					}
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[BLACK][from] & pos->occupancies[WHITE];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h2 >= from && from >= a2)
						{
							add_capture_promotion(list, encode_move(from, to, piece, BQ, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BR, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BB, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (pos->enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[BLACK][from] & (1ULL << pos->enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0), pos);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case BK:
				if (pos->castle & BKCA)
				{
					// check empty
					if (!get_bit(pos->occupancies[BOTH], f8) && !get_bit(pos->occupancies[BOTH], g8))
					{
						// check safety
						if (!is_square_attacked(f8, WHITE, pos) && !is_square_attacked(e8, WHITE, pos))
						{
							add_quiet(list, encode_move(e8, g8, piece, NO_PIECE, 0, 0, 0, 1), pos);
						}

					}
				}
				if (pos->castle & BQCA)
				{
					// check empty
					if (!get_bit(pos->occupancies[BOTH], d8) && !get_bit(pos->occupancies[BOTH], c8) && !get_bit(pos->occupancies[BOTH], b8))
					{
						// check safety
						if (!is_square_attacked(d8, WHITE, pos) && !is_square_attacked(e8, WHITE, pos))
						{
							add_quiet(list, encode_move(e8, c8, piece, NO_PIECE, 0, 0, 0, 1), pos);
						}

					}
				}
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = knight_attacks[from] & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_bishop_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_rook_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
					attacks = (get_queen_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						else
						{
							add_quiet(list, encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0), pos);
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
static inline void generate_captures(Moves_list *list, BOARD*pos)
{
	list->count = 0;


	int from, to;

	Bitboard bitboard, attacks;

	Piece start_piece = (pos->side == WHITE) ? WP : BP;
	Piece end_piece = (pos->side == WHITE) ? WK : BK;

	for (Piece piece = start_piece; piece <= end_piece; ++piece)
	{
		bitboard = pos->bitboards[piece];

		if (pos->side == WHITE)
		{
			switch (piece)
			{
			case WP:
				while (bitboard) // for each piece in bitboard
				{
					from = bitscan_forward(bitboard);
					
					// initialize pawn attacks bitboard
					attacks = pawn_attacks[WHITE][from] & pos->occupancies[BLACK];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h7 >= from && from >= a7)
						{
							// 4 promotions
							add_capture_promotion(list, encode_move(from, to, piece, WQ, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WR, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WB, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, WN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (pos->enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[WHITE][from] & (1ULL << pos->enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0), pos);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case WK:
				
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (knight_attacks[from]) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_bishop_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_rook_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_queen_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[WHITE]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[BLACK], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = pawn_attacks[BLACK][from] & pos->occupancies[WHITE];

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (h2 >= from && from >= a2)
						{
							add_capture_promotion(list, encode_move(from, to, piece, BQ, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BR, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BB, 1, 0, 0, 0));
							add_capture_promotion(list, encode_move(from, to, piece, BN, 1, 0, 0, 0));
						}
						else
						{
							// one square ahead move
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
						}
						pop_bit(attacks, to);
					}

					// en passant
					if (pos->enpassant != NO_SQUARE)
					{
						Bitboard enpassant_attacks = pawn_attacks[BLACK][from] & (1ULL << pos->enpassant);
						if (enpassant_attacks)
						{
							to = bitscan_forward(enpassant_attacks);
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 1, 0), pos);
						}
					}

					pop_bit(bitboard, from);
				}
				break;
			case BK:
				
				while (bitboard)
				{
					from = bitscan_forward(bitboard);
					attacks = (king_attacks[from]) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = knight_attacks[from] & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_bishop_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_rook_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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
					attacks = (get_queen_attacks(from, pos->occupancies[BOTH])) & (~pos->occupancies[BLACK]);

					while (attacks)
					{
						to = bitscan_forward(attacks);
						if (get_bit(pos->occupancies[WHITE], to))
						{
							add_capture(list, encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0), pos);
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



static inline int make_move(int move, BOARD * pos)
{
	++pos->fiftymove;
	++pos->ply;
	++pos->hisPly;

	if (move == NULL_MOVE)
	{
		pos->enpassant = NO_SQUARE;
		pos->side ^= 1;
		return TRUE;
	}

	copy_board(pos);
	const int from = get_move_from(move);
	const int to = get_move_to(move);
	const int piece = get_move_piece(move);
	const int promoted_piece = get_move_promotion(move);
	const int capture = get_move_capture(move);
	const int double_PP = get_move_double(move);
	const int enpass = get_move_enpas(move);
	const int castling = get_move_castle(move);

	const int opp_side = pos->side ^ 1;
	

	pop_bit(pos->bitboards[piece], from);
	set_bit(pos->bitboards[piece], to);
	pop_bit(pos->occupancies[pos->side], from);
	set_bit(pos->occupancies[pos->side], to);


	if (capture)
	{
		pos->fiftymove = 0;
		int start_piece, end_piece;
		if (pos->side == WHITE)
		{
			start_piece = BP;
			end_piece = BK;
		}
		else
		{
			start_piece = WP;
			end_piece = WK;
		}

		for (int bb_piece = start_piece; bb_piece <= end_piece; ++bb_piece)
		{
			if (get_bit(pos->bitboards[bb_piece], to))
			{
				pop_bit(pos->bitboards[bb_piece], to);
				pop_bit(pos->occupancies[opp_side], to);
				break;
			}
		}
	}
	if (promoted_piece != NO_PIECE)
	{
		pop_bit(pos->bitboards[(pos->side == WHITE) ? WP : BP], to);
		set_bit(pos->bitboards[promoted_piece], to);
		pos->enpassant = NO_SQUARE;
	}
	else if (enpass)
	{
		if (pos->side == WHITE)
		{
			pop_bit(pos->bitboards[BP], to + 8);
			pop_bit(pos->occupancies[BLACK], to + 8);
		}
		else
		{
			pop_bit(pos->bitboards[WP], to - 8);
			pop_bit(pos->occupancies[WHITE], to - 8);
		}
		pos->enpassant = NO_SQUARE;
	}
	else if (double_PP)
	{
		(pos->side == WHITE) ? (pos->enpassant = to + 8) : (pos->enpassant = to - 8);
	}
	else
	{
		pos->enpassant = NO_SQUARE;
		if (castling)
		{
			switch (to)
			{
			case (g1):
				pop_bit(pos->bitboards[WR], h1);
				pop_bit(pos->occupancies[WHITE], h1);
				set_bit(pos->bitboards[WR], f1);
				set_bit(pos->occupancies[WHITE], f1);
				break;
			case (c1):
				pop_bit(pos->bitboards[WR], a1);
				pop_bit(pos->occupancies[WHITE], a1);
				set_bit(pos->bitboards[WR], d1);
				set_bit(pos->occupancies[WHITE], d1);
				break;
			case (g8):
				pop_bit(pos->bitboards[BR], h8);
				pop_bit(pos->occupancies[BLACK], h8);
				set_bit(pos->bitboards[BR], f8);
				set_bit(pos->occupancies[BLACK], f8);
				break;
			case (c8):
				pop_bit(pos->bitboards[BR], a8);
				pop_bit(pos->occupancies[BLACK], a8);
				set_bit(pos->bitboards[BR], d8);
				set_bit(pos->occupancies[BLACK], d8);
				break;
			}
		}
	}

	if (piece % 6 == PAWN)
	{
		pos->fiftymove = 0;
	}

	pos->castle &= castling_rights[from];
	pos->castle &= castling_rights[to];

	pos->occupancies[BOTH] = pos->occupancies[WHITE] | pos->occupancies[BLACK];

	pos->side ^= 1;


	if (is_square_attacked((pos->side == WHITE) ? bitscan_forward(pos->bitboards[BK]) : bitscan_forward(pos->bitboards[WK]), pos->side, pos))
	{
		take_board(pos);
		return FALSE;
	}

	return TRUE;
}






#endif