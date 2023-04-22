#include "bitboard.h"


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














