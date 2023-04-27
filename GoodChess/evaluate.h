#ifndef EVALUATE_H_INCLUDED
#define EVALUATE_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "bitboard.h"
#include "board.h"


/* DEFINITIONS */
static const int material_score[12] =
{
	PawnValue, KnightValue, BishopValue, RookValue, QueenValue, 0, -PawnValue, -KnightValue, -BishopValue, -RookValue, -QueenValue, 0
};
// pawn positional score
static const int pawn_score[64] =
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
static const int knight_score[64] =
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
static const int bishop_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
static const int rook_score[64] =
{
    30,  30,  30,  30,  30,  30,  30,  30,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
static const int king_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
static const int mirror_score[128] =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// MVV LVA [attacker][victim]
static int mvv_lva[6][6] = {
    105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600
};



/* MACROS */


/* FUNCTIONS */
static inline int score_move(int move)
{
    if (get_move_capture(move))
    {
        const int Piece_type = get_move_pieceType(move);
        // pick up bitboard piece index ranges depending on side
        int start_piece, end_piece;

        // pick up side to move
        if (side == WHITE) { start_piece = BP; end_piece = BQ; }
        else { start_piece = WP;  end_piece = WQ; }

        // loop over bitboards opposite to the current side to move
        for (int bb_piece = start_piece; bb_piece <= end_piece;)
        {
            ++bb_piece;
            // if there's a piece on the target square
            if (get_bit(bitboards[bb_piece], get_move_to(move)))
            {
                // remove it from corresponding bitboard
                start_piece = bb_piece;
                break;
            }
        }

        // score move by MVV LVA lookup [source piece][target piece]
        printf("\n%d, %d\n", Piece_type, start_piece);
        return mvv_lva[Piece_type][start_piece];
    }
    else
    {

    }
    return 0;
}

// print move scores
static inline void print_move_scores(Moves_list* move_list)
{
    printf("     Move scores:\n\n");

    // loop over moves within a move list
    for (int count = 0; count < move_list->count; count++)
    {
        printf("     move: ");
        printf("%s", Pr_move(move_list->moves[count]));
        printf(" score: %d\n", score_move(move_list->moves[count]));
    }
}



static inline int evaluate_material()
{
	int score = 0;

	for (int bb_piece = WP; bb_piece <= BK; ++bb_piece)
	{
		score += (count_bits(bitboards[bb_piece]) * material_score[bb_piece]);
	}

	return (side == WHITE) ? score : -score;
};


// position evaluation
static inline int evaluate_position()
{
    int score = 0;
    Bitboard bitboard;
    int piece, square;

    for (int bb_piece = WP; bb_piece <= WK; ++bb_piece)
    {
        // init piece bitboard copy
        bitboard = bitboards[bb_piece];
        // loop over pieces within a bitboard
        while (bitboard)
        {
            // init piece
            piece = bb_piece;

            // init square
            square = bitscan_forward(bitboard);
            score += material_score[piece];
            switch (piece)
            {
                // evaluate white pieces
            case WP: score += pawn_score[square]; break;
            case WN: score += knight_score[square]; break;
            case WB: score += bishop_score[square]; break;
            case WR: score += rook_score[square]; break;
            case WK: score += king_score[square]; break;
            }


            // pop ls1b
            pop_bit(bitboard, square);
        }
    }

    for (int bb_piece = BP; bb_piece <= BK; ++bb_piece)
    {
        // init piece bitboard copy
        bitboard = bitboards[bb_piece];
        // loop over pieces within a bitboard
        while (bitboard)
        {
            // init piece
            piece = bb_piece;

            // init square
            square = bitscan_forward(bitboard);
            score += material_score[piece];
            switch (piece)
            {

                // evaluate black pieces
            case BP: score -= pawn_score[mirror_score[square]]; break;
            case BN: score -= knight_score[mirror_score[square]]; break;
            case BB: score -= bishop_score[mirror_score[square]]; break;
            case BR: score -= rook_score[mirror_score[square]]; break;
            case BK: score -= king_score[mirror_score[square]]; break;
            }


            // pop ls1b
            pop_bit(bitboard, square);
        }
    }

    // return final evaluation based on side
    return (side == WHITE) ? score : -score;
}


inline int eval()
{
	return evaluate_position();
}




#endif