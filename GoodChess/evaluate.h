#ifndef EVALUATE_H_INCLUDED
#define EVALUATE_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "bitboard.h"
#include "board.h"


/* DEFINITIONS */
static const int material_score[12] =
{
	100, 325, 325, 500, 1000, 0, -100, -325, -325, -500, -1000, 0
};


/* MACROS */


/* FUNCTIONS */
static inline int evaluate_material()
{
	int score = 0;

	for (int bb_piece = WP; bb_piece <= BK; ++bb_piece)
	{
		score += (count_bits(bitboards[bb_piece]) * material_score[bb_piece]);
	}

	return (side == WHITE) ? score : -score;
};
inline int eval()
{
	return evaluate_material();
}




#endif