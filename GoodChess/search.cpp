#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include "misc.h"
#include "board.h"
#include "pvtable.h"
#include <iostream>
#include <algorithm>


// Clear search information for a new search
static void ClearForSearch(Search_info* info, BOARD*pos, HASHTABLE * table) {

	// Define indices
	int index = 0;
	int index2 = 0;

	// Clear history
	for (index = 0; index < PIECE_NUMB; ++index) {
		for (index2 = 0; index2 < SQUARE_NUMB; ++index2) {
			pos->history_heuristic[index][index2] = 0;
		}
	}

	// Clear killer moves
	for (index = 0; index < 2; ++index) {
		for (index2 = 0; index2 < MAX_PLY; ++index2) {
			pos->killer_moves[index][index2] = 0;
		}
	}

	//table->overWrite = 0;
	table->hit = 0;
	table->cut = 0;
	++table->currentAge; // Incremement current hash table age
	pos->ply = 0; // Set search ply to 0

	// Reset search info
	info->stopped = 0;
	info->nodes = 0;
	//info->fh = 0;
	//info->fhf = 0;

}



static inline bool InCheck(BOARD*pos)
{
	return is_square_attacked((pos->side == BLACK) ? bitscan_forward(pos->bitboards[BK]) : bitscan_forward(pos->bitboards[WK]), (pos->side ^ 1), pos );
}


static inline int Quiescence(int alpha, int beta, BOARD*pos)
{


	int Score = eval(pos);
	// Standing pat
	if (Score >= beta) {
		return beta;
	}
	if (Score > alpha) {
		alpha = Score;
	}
	Moves_list list[1];
	generate_captures(list, pos);

	Score = -VALUE_INFINITE;
	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		sort_moves(MoveNum, list);
		copy_board(pos);
		if (!make_move(list->moves[MoveNum].move, pos))
		{
			continue;
		}


		Score = -Quiescence(-beta, -alpha, pos); // Recursively call function

		take_board(pos);
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



static inline int NegaMax(int alpha, int beta, int depth, Search_info *info, BOARD*pos, HASHTABLE *table)
{
	bool Check = InCheck(pos);
	//if (Check)
	//{
	//	depth = std::max(1, depth + 1);
	//}


	if (depth <= 0)
	{
		++info->nodes;
		return Quiescence(alpha, beta, pos);
	}

	// Check for draw or repetition
	if ((pos->fiftymove >= 100) && pos->ply) {
		return VALUE_DRAW;
	}



	Moves_list list[1];
	generate_moves(list, pos);

	int legal_moves = 0;
	int BestMove = 0;
	int Score = -VALUE_INFINITE;
	int OldAlpha = alpha;

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		sort_moves(MoveNum, list);

		copy_board(pos);
		if (!make_move(list->moves[MoveNum].move, pos))
		{
			continue;
		}
		int move = list->moves[MoveNum].move;
		++legal_moves;

		Score = -NegaMax(-beta, -alpha, depth - 1, info, pos, table);

		take_board(pos);

		if (Score > alpha)
		{

			//if (not get_move_capture(move))
			//{
			//	history_heuristic[get_move_piece(move)][get_move_to(move)] += depth;
			//}
			pos->history_heuristic[get_move_piece(move)][get_move_to(move)] += depth;

			BestMove = move;
			alpha = Score;


			if (Score >= beta)
			{
				//if (not get_move_capture(move))
				//{
				//	killer_moves[1][ply] = killer_moves[0][ply];
				//	killer_moves[0][ply] = move;
				//}
				pos->killer_moves[1][pos->ply] = pos->killer_moves[0][pos->ply];
				pos->killer_moves[0][pos->ply] = move;
				return beta;
			}
		}
	}
	
	if (legal_moves == 0)
	{
		if (Check)
		{
			return (-VALUE_MATE + pos->ply); // Checkmate
		}
		else
		{
			return VALUE_DRAW; // Stalemate
		}
	}

	if (alpha != OldAlpha)
	{
		StoreHashEntry(pos, table, BestMove, alpha, HFEXACT, depth); // Store hash entry of position
	}
	else {
		StoreHashEntry(pos, table, BestMove, alpha, HFALPHA, depth); // Store hash entry of position
	}

	return alpha;
}


static inline void iterative_deepen(Search_info *info, BOARD*pos, HASHTABLE *table)
{
	int bestScore = -VALUE_INFINITE;
	int pvMoves = 0;


	for (int currentDepth = 1; currentDepth <= info->S_depth; ++currentDepth)
	{
		info->bestMove = 0;
		info->nodes = 0;


		bestScore = NegaMax(-VALUE_INFINITE, VALUE_INFINITE, currentDepth, info, pos, table);

		pvMoves = GetPvLine(currentDepth, pos, table);
		info->bestMove = pos->pvArray[0];

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
		for (int count = 0; count < pvMoves; ++count)
		{
			// print PV move
			std::cout << Pr_move(pos->pvArray[count]) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "bestmove " << Pr_move(info->bestMove) << "\n";
}



void search_position(Search_info *info, BOARD*pos, HASHTABLE * table)
{
	ClearForSearch(info, pos, table);
	iterative_deepen(info, pos, table);
}











