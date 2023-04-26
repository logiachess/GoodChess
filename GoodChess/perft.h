#ifndef PERFT_H_INCLUDED
#define PERFT_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "movegen.h"


/* DEFINITIONS */
long nodes;


/* MACROS */


/* FUNCTIONS */
static inline void perft_driver(int depth)
{
	if (depth == 0)
	{
		++nodes;
		return;
	}

	Moves_list list[1];
	generate_moves(list);

	for (int move_count = 0; move_count < list->count; ++move_count)
	{
		copy_board();
		if (!make_move(list->moves[move_count]))
		{
			continue;
		}
		perft_driver(depth - 1);
		take_board();
	}
}










#endif