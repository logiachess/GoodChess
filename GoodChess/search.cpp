#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include "misc.h"
#include "board.h"
#include <iostream>
#include <algorithm>


int killer_moves[2][MAX_PLY];
int history_heuristic[PIECE_NUMB][SQUARE_NUMB];


// Clear search information for a new search
static void ClearForSearch(Search_info* info) {

	// Define indices
	int index = 0;
	int index2 = 0;

	// Clear history
	for (index = 0; index < PIECE_NUMB; ++index) {
		for (index2 = 0; index2 < SQUARE_NUMB; ++index2) {
			history_heuristic[index][index2] = 0;
		}
	}

	// Clear killer moves
	for (index = 0; index < 2; ++index) {
		for (index2 = 0; index2 < MAX_PLY; ++index2) {
			killer_moves[index][index2] = 0;
		}
	}

	////table->overWrite = 0;
	//table->hit = 0;
	//table->cut = 0;
	//++table->currentAge; // Incremement current hash table age
	ply = 0; // Set search ply to 0

	// Reset search info
	info->stopped = 0;
	info->nodes = 0;
	//info->fh = 0;
	//info->fhf = 0;

}



static inline bool InCheck()
{
	return is_square_attacked((side == BLACK) ? bitscan_forward(bitboards[BK]) : bitscan_forward(bitboards[WK]), (side ^ 1) );
}


static inline int Quiescence(int alpha, int beta)
{


	int Score = eval();
	// Standing pat
	if (Score >= beta) {
		return beta;
	}
	if (Score > alpha) {
		alpha = Score;
	}
	Moves_list list[1];
	generate_captures(list);

	Score = -VALUE_INFINITE;
	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		sort_moves(MoveNum, list);
		copy_board();
		if (!make_move(list->moves[MoveNum].move))
		{
			continue;
		}


		Score = -Quiescence(-beta, -alpha); // Recursively call function

		take_board();
		if (Score >= beta)
		{
			return beta;
		}
		if (Score > alpha)
		{
			alpha = Score;
		}
	}

	return alpha;
}



static inline int NegaMax(int alpha, int beta, int depth, Search_info *info)
{
	bool Check = InCheck();
	//if (Check)
	//{
	//	depth = std::max(1, depth + 1);
	//}


	if (depth <= 0)
	{
		++info->nodes;
		return Quiescence(alpha, beta);
	}


	int Score = -VALUE_INFINITE;


	Moves_list list[1];
	generate_moves(list);

	int legal_moves = 0;

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		sort_moves(MoveNum, list);
		copy_board();
		if (!make_move(list->moves[MoveNum].move))
		{
			continue;
		}
		++legal_moves;
		Score = -NegaMax(-beta, -alpha, depth - 1, info);

		take_board();

		if (Score >= beta)
		{
			killer_moves[1][ply] = killer_moves[0][ply];
			killer_moves[0][ply] = list->moves[MoveNum].move;
			return beta;
		}

		if (Score > alpha)
		{
			history_heuristic[get_move_piece(list->moves[MoveNum].move)][get_move_to(list->moves[MoveNum].move)] += depth;

			alpha = Score;
			if (ply == 0)
			{
				info->bestMove = list->moves[MoveNum].move;
			}
		}
	}
	
	if (legal_moves == 0)
	{
		if (Check)
		{
			return (-VALUE_MATE + ply); // Checkmate
		}
		else
		{
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


		bestScore = NegaMax(-VALUE_INFINITE, VALUE_INFINITE, currentDepth, info);



		long time = get_time_ms() - info->starttime;
		U64 nps = info->nodes / (time + !time) * 1000;

		if (bestScore > -VALUE_MATE && bestScore < VALUE_MATED_IN_MAX_PLY)
		{
			std::cout << "info score mate " << -(bestScore + VALUE_MATE) / 2 << " depth " << currentDepth << " nodes " << info->nodes <<
				" nps " << nps << " time " << time << " pv ";
		}
		else if (bestScore > VALUE_MATE_IN_MAX_PLY && bestScore < VALUE_MATE)
		{
			std::cout << "info score mate " << (VALUE_MATE - bestScore) / 2 + 1 << " depth " << currentDepth << " nodes " << info->nodes <<
				" nps " << nps << " time " << time << " pv ";
		}
		else {
			std::cout << "info score cp " << bestScore << " depth " << currentDepth << " nodes " << info->nodes <<
				" nps " << nps << " time " << time << " pv ";
		}
		std::cout << Pr_move(info->bestMove) << std::endl;
	}
	std::cout << "bestmove " << Pr_move(info->bestMove) << "\n";
}



void search_position(Search_info *info)
{
	ClearForSearch(info);
	iterative_deepen(info);
}











