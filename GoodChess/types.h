#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


/* HEADERS */
#include <cstdint>
#include <cstdlib>
#include <algorithm>


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
	a1, b1, c1, d1, e1, f1, g1, h1, NO_SQUARE
};

enum Boolen {
	FALSE, TRUE
};

enum Color {
  WHITE, BLACK, BOTH
};


enum File : int {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};


enum Rank : int {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};


enum PieceType {
	NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};


enum Piece {
	WP, WN, WB, WR, WQ, WK,
	BP, BN, BB, BR, BQ, BK,
	NO_PIECE
};


enum  Castlingrights
{
	WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8
};




#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }


ENABLE_INCR_OPERATORS_ON(Piece)
ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

constexpr Square operator+(Square s, int d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, int d) { return Square(int(s) - int(d)); }
inline Square& operator+=(Square& s, int d) { return s = s + d; }
inline Square& operator-=(Square& s, int d) { return s = s - d; }



#endif