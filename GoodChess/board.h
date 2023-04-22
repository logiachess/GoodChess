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





#endif