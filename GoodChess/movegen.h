#ifndef MOVEGEN_H_INCLUDED
#define MOVEGEN_H_INCLUDED


/* HEADERS */
#include "types.h"


/* DEFINITIONS */


/* MACROS */


/* FUNCTIONS */
static inline Bitboard get_bishop_attacks(int square, Bitboard occupancy);
static inline Bitboard get_rook_attacks(int square, Bitboard occupancy);
static inline Bitboard get_queen_attacks(int square, Bitboard occupancy);
static inline int is_square_attacked(int square, int side);

// static inline
extern void generate_moves();





#endif