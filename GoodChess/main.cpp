#include <stdio.h>
#include "bitboard.h"
#include "board.h"






int main() {

	U64 bitboard = 0ULL;

	set_bit(bitboard, e4);
	print_bitboard(bitboard);

	clr_bit(bitboard);
	print_bitboard(bitboard);

	getchar();
	return 0;
}




















