#ifndef ATTACKS_H_INCLUDED
#define ATTACKS_H_INCLUDED


/* HEADERS */
#include "types.h"
#include "board.h"
#include "bitboard.h"


/* DEFINITIONS */
extern const Bitboard pawn_attacks[2][64];
extern const Bitboard knight_attacks[64];
extern const Bitboard king_attacks[64];
extern Bitboard bishop_attacks[64][512];
extern Bitboard rook_attacks[64][4096];


extern Bitboard bishop_masks[64];
extern Bitboard rook_masks[64];

extern const Bitboard bishop_occupancy[64];
extern const Bitboard rook_occupancy[64];

extern const int bishop_relevant_occupancy[64];
extern const int rook_relevant_occupancy[64];


constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;

constexpr Bitboard FileHGBB = FileHBB | FileGBB;
constexpr Bitboard FileABBB = FileABB | FileBBB;

constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);

/* MACROS */


/* FUNCTIONS */
extern Bitboard mask_pawn_attacks(int side, int square);
extern Bitboard mask_knight_attacks(int square);
extern Bitboard mask_king_attacks(int square);
extern Bitboard mask_bishop_attacks(int square, Bitboard occupied);
extern Bitboard mask_rook_attacks(int square, Bitboard occupied);

extern Bitboard mask_bishop_occupancy(int square);
extern Bitboard mask_rook_occupancy(int square);
extern void init_sliders_attacks(PieceType pt);

static inline Bitboard get_bishop_attacks(int square, Bitboard occupancy);
static inline Bitboard get_rook_attacks(int square, Bitboard occupancy);
static inline Bitboard get_queen_attacks(int square, Bitboard occupancy);
static inline int is_square_attacked(int square, int side);






#endif