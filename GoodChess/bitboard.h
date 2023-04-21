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


/* FUNCTIONS */
extern void print_bitboard(U64 bitboard);






