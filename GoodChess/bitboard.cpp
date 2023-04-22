#include "bitboard.h"
#include <stdlib.h>
#include <random>




const char* sq_to_coord[64] =
{
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

const int bishop_relevant_occupancy[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};


const int rook_relevant_occupancy[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

void init_bitboards()
{

}


void print_bitboard(U64 bitboard)
{

	printf("\n");
	// Loop over board ranks
	for (int rank = 0; rank < 8; ++rank)
	{
		
		// Loop over board files
		for (int file = 0; file < 8; ++file)
		{

			// Convert file and rank to square index
			int square = rank * 8 + file;

			// Print ranks
			if (!file)
			{

				printf(" %d  ", 8 - rank);
			}

			printf(" %d", (get_bit(bitboard, square) ? 1 : 0));
		}
		printf("\n");

	}

	// Print files
	printf("\n     a b c d e f g h \n\n");

	// Print bitboard as U64 value
	printf("\n     Bitboard: %llu\n", bitboard);


}


Bitboard set_occupancy(int index, Bitboard attack_mask)
{

	// occupancy map
	Bitboard occupancy = 0ULL;
	int bits = count_bits(attack_mask);

	for (int bitcount = 0; bitcount < bits; ++bitcount)
	{
		int square = bitscan_forward(attack_mask);

		pop_bit(attack_mask, square);

		if (index & (1 << bitcount))
		{
			occupancy |= (1ULL << square);
		}
	}

	return occupancy;
}











