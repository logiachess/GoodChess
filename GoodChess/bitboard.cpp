#include "bitboard.h"


void print_bitboard(U64 bitboard) {

	// Loop over board ranks
	for (int rank = 0; rank < 8; ++rank) {
		
		// Loop over board files
		for (int file = 0; file < 8; ++file) {

			// Convert file and rank to square index
			int square = rank * 8 + file;

			printf(" %d ", square);
		}
		printf("\n");
	}
}










