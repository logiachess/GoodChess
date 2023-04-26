#ifndef PERFT_H_INCLUDED
#define PERFT_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "movegen.h"


/* DEFINITIONS */
long long nodes;


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

static inline void perft_test(int depth)
{
    nodes = 0;
    printf("\n     Performance test\n\n");
    printf("\n     Move:    Nodes:\n");

    // create move list instance
    Moves_list move_list[1];

    // generate moves
    generate_moves(move_list);

    // init start time
    long start = get_time_ms();

    // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        // preserve board state
        copy_board();

        // make move
        if (!make_move(move_list->moves[move_count]))
            // skip to the next move
            continue;

        // cummulative nodes
        long long cummulative_nodes = nodes;

        // call perft driver recursively
        perft_driver(depth - 1);

        // old nodes
        long old_nodes = nodes - cummulative_nodes;

        // take back
        take_board();

        // print move
        printf("     %s    %lld\n", Pr_move(move_list->moves[move_count]), old_nodes);
    }

    // print results
    printf("\n    Depth: %d\n", depth);
    printf("    Nodes: %lld\n", nodes);
    printf("     Time: %ld\n\n", get_time_ms() - start);
}










#endif