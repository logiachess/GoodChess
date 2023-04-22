#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED


/* HEADERS */
#include "types.h"


/* DEFINITIONS */
extern const U64 rook_magics[64];
extern const U64 bishop_magics[64];


/* MACROS */


/* FUNCTIONS */
extern unsigned int get_random_U32_number();
extern U64 get_random_U64_numbers();
extern U64 generate_magic_number();
extern U64 find_magic_number(int square, int relevant_ocucpancy, PieceType pt);











#endif