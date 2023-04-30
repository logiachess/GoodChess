#ifndef BITBOARD_H_INCLUDED
#define BITBOARD_H_INCLUDED


/* HEADERS */
#include "stdio.h"
#include "types.h"
//#include <map>


/* DEFINITIONS */


/* MACROS */
#define get_bit(bitboard, square) ((bitboard) & (1ULL << square) ) // Check if bit value is one
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << square) ) // Set bit to one
#define pop_bit(bitboard, square) ( (bitboard) &= ~( 1ULL << (square) ) ) // Pop bit
#define clr_bit(bitboard) ((bitboard) &= (bitboard - 1)) // Clear bitboard

// bitmagic
#define count_bits(bitboard) ( __builtin_popcountll(bitboard) )
#define bitscan_forward(bitboard) ( __builtin_ctzll(bitboard) ) // least significant
#define bitscan_reverse(bitboard) ( (63 - __builtin_clzll(bitboard) ) ) // most significant





/* FUNCTIONS */
extern void print_bitboard(Bitboard bitboard);
extern Bitboard set_occupancy(int index, int bits, Bitboard attack_mask);







#endif