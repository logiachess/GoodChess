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
extern int hisPly;
extern int ply;
extern int fiftymove;


/* MACROS */
#define copy_board()                                                      \
    U64 bitboards_copy[12], occupancies_copy[3];                          \
    int side_copy, enpassant_copy, castle_copy;                           \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \

// restore board state
#define take_board()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;    \
    --ply; --hisPly; --fiftymove;                                           \


/* FUNCTIONS */
extern void print_board();
extern void parse_fen(const std::string& command);
extern char* Pr_move(int move);
extern void print_moves_list(const Moves_list *moves_list);



#endif