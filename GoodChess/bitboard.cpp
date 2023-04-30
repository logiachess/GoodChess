#include "bitboard.h"




void print_bitboard(Bitboard bitboard)
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


Bitboard set_occupancy(int index, int bits, Bitboard attack_mask)
{

	// occupancy map
	Bitboard occupancy = 0ULL;

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











