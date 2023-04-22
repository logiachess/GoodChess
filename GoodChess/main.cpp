#include <stdio.h>
#include "init.h"
#include "bitboard.h"
#include "board.h"
#include "attacks.h"


int main()
{
	InitAll();

	Bitboard attack_mask = mask_rook_occupancy(a1);

	for (int index = 0; index <= 4095; ++index) {
		print_bitboard(set_occupancy(index, attack_mask));
		getchar();
	}



	return 0;
}




















