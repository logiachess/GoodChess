#pragma once


/* HEADERS */
#include "types.h"
#include "board.h"
#include "bitboard.h"


/* DEFINITIONS */
// pawn attacks table
extern Bitboard pawn_attacks[2][64];

const U64 not_a_file = 18374403900871474942ULL;
const U64 not_h_file = 9187201950435737471ULL;
const U64 not_hg_file = 4557430888798830399ULL;
const U64 not_ab_file = 18229723555195321596ULL;


/* MACROS */


/* FUNCTIONS */
// generate pawn attacks
extern U64 mask_pawn_attacks(int side, int square);






