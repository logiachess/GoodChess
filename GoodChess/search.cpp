#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include "misc.h"
#include "board.h"
#include <iostream>


static inline bool InCheck()
{
	return is_square_attacked((side == BLACK) ? bitscan_forward(bitboards[BK]) : bitscan_forward(bitboards[WK]), side ^= 1);
}


static inline int NegaMax(int alpha, int beta, int depth, Search_info *info, int c)
{
	if (depth == 0) {
		++info->nodes;
		return eval();
	}
	int Score = -VALUE_INFINITE;

	Moves_list list[1];
	generate_moves(list);

	int legal_moves = 0;

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		copy_board(); // How to fix
		if (!make_move(list->moves[MoveNum]))
		{
			continue;
		}
		++legal_moves;

		Score = -NegaMax(-beta, -alpha, depth - 1, info, c);
		take_board();

		if (Score >= beta)
		{
			return beta;
		}

		if (Score > alpha)
		{
			alpha = Score;
			if (depth == c) {
				info->bestMove = list->moves[MoveNum];
			}
		}
	}
	
	if (legal_moves == 0)
	{
		if (InCheck) {
			return (-VALUE_MATE); // Checkmate
		}
		else {
			return VALUE_DRAW; // Stalemate
		}
	}

	return alpha;
}


static inline void iterative_deepen(Search_info *info)
{
	int bestScore = -VALUE_INFINITE;
	int currentDepth = 1;
	for (; currentDepth <= info->S_depth; ++currentDepth)
	{
		info->bestMove = 0;
		info->nodes = 0;


		bestScore = NegaMax(-VALUE_INFINITE, VALUE_INFINITE, currentDepth, info, currentDepth);



		long time = get_time_ms() - info->starttime;
		U64 nps = info->nodes / (time + !time) * 1000;

		std::cout << "info score cp " << bestScore << " depth " << currentDepth << " nodes " << info->nodes <<
			" nps " << nps << " time " << time << " pv " << Pr_move(info->bestMove) << std::endl;
	}
	std::cout << "bestmove " << Pr_move(info->bestMove) << "\n";
}



void search_position(Search_info *info)
{

	iterative_deepen(info);
}











