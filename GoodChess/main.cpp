#include <stdio.h>
#include "init.h"
#include "bitboard.h"
#include "board.h"
#include "attacks.h"


int main()
{
	InitAll();

	print_bitboard((U64)(get_random_U32_number()));
	print_bitboard((U64)(get_random_U32_number() & 0xFFFF));
	print_bitboard(get_random_U64_numbers());
	print_bitboard(generate_magic_number());	



	return 0;
}




















