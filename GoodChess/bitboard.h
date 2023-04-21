#pragma once


// Include
#include "stdio.h"


// Definitions
#define U64 unsigned long long int


// Macros
#define get_bit(bitboard, square) (bitboard & (1ULL << square) ) // Check if bit value is one
#define set_bit(bitboard, square) (bitboard |= (1ULL << square) ) // Set bit to one
#define pop_bit(bitboard, square) ( (bitboard) &= ~( 1ULL << (square) ) ) // Pop bit
#define clr_bit(bitboard) ((bitboard) &= (bitboard - 1)) // Clear bitboard


// Functions
extern void print_bitboard(U64 bitboard);






