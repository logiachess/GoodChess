#include <stdio.h>
#include "init.h"
#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "magic.h"
#include "movegen.h"
	

#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "


int main()
{
	InitAll();

	parse_fen("r3k2r/p1ppqpb1/bn2pnp1/2pPN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq c6 0 1 ");
	print_board();


	Moves_list move_list[1];
	generate_moves(move_list);

	for (int move_count = 0; move_count < move_list->count; ++move_count)
	{
		int move = move_list->moves[move_count];
		copy_board();
		make_move(move);
		print_board();
		print_bitboard(occupancies[BOTH]);
		getchar();
		take_board();
		print_board();
		print_bitboard(occupancies[BOTH]);
		getchar();
	}
		

	return 0;
}




















