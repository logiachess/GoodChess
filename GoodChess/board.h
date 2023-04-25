#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "misc.h"
#include <map>


/* DEFINITIONS */
extern std::map<char, int> char_pieces;

extern const char ascii_pieces[];
extern const char* sq_to_coord[64];


extern int side;
extern int enpassant;
extern int castle;
extern Bitboard bitboards[12];
extern Bitboard occupancies[3];


/* MACROS */


/* FUNCTIONS */
extern void print_board();
extern void parse_fen(const std::string& command);
extern char* Pr_move(int move);
extern void print_moves_list(const Moves_list *moves_list);
static inline Board_copy copy_board()
{
	Board_copy board_copy;
	memcpy(board_copy.bitboards, bitboards, SIZEOF_BITBOARD);
	memcpy(board_copy.occupancies, occupancies, SIZEOF_OCCUPANCIES);
	board_copy.side_copy = side, board_copy.enpassant_copy = enpassant, board_copy.castle_copy = castle;
	return board_copy;
}
static inline void restore_board(Board_copy board_copy)
{
	memcpy(bitboards, board_copy.bitboards, SIZEOF_BITBOARD);
	memcpy(occupancies, board_copy.occupancies, SIZEOF_OCCUPANCIES);
	side = board_copy.side_copy, enpassant = board_copy.enpassant_copy, castle = board_copy.castle_copy;

}



#endif