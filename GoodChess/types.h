#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


/* HEADERS */


/* DEFINITIONS*/
#define U64 unsigned long long int
using Bitboard = U64;

constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY = 246;


enum Square	// board squares
{
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1
};

enum Color {
  WHITE, BLACK, COLOR_NB = 2
};


enum File : int {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};


enum Rank : int {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};


enum PieceType {
	NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};


enum Piece {
	NO_PIECE,
	WP, WN, WB, WR, WQ, WK,
	BP, BN, BB, BR, BQ, BK
};



#endif