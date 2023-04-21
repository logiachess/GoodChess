#pragma once


/* HEADERS */
#include "stdio.h"
#include "types.h"


/* DEFINITIONS */


/* MACROS */
#define get_bit(bitboard, square) (bitboard & (1ULL << square) ) // Check if bit value is one
#define set_bit(bitboard, square) (bitboard |= (1ULL << square) ) // Set bit to one
#define pop_bit(bitboard, square) ( (bitboard) &= ~( 1ULL << (square) ) ) // Pop bit
#define clr_bit(bitboard) ((bitboard) &= (bitboard - 1)) // Clear bitboard

// bitmagic
#define count_bits(bitboard) ( __builtin_popcountll(bitboard) )
#define bitscan_forward(bitboard) ( __builtin_ctzll(bitboard) )
#define bitscan_reverse(bitboard) ( (63 - __builtin_clzll(bitboard) ) )


/* FUNCTIONS */
extern void print_bitboard(U64 bitboard);






