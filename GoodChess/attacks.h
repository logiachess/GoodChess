#pragma once


/* HEADERS */
#include "types.h"
#include "board.h"
#include "bitboard.h"


/* DEFINITIONS */
extern Bitboard pawn_attacks[2][64];
extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];

const Bitboard not_a_file = 18374403900871474942ULL;
const Bitboard not_h_file = 9187201950435737471ULL;
const Bitboard not_hg_file = 4557430888798830399ULL;
const Bitboard not_ab_file = 18229723555195321596ULL;


/* MACROS */


/* FUNCTIONS */
extern Bitboard mask_pawn_attacks(int side, int square);
extern Bitboard mask_knight_attacks(int square);
extern Bitboard mask_king_attacks(int square);






