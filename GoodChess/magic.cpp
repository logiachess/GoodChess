#include "magic.h"


unsigned int random_state = 1804289383;

unsigned int get_random_U32_number()
{
	unsigned int number = random_state;

	number ^= number << 13;
	number ^= number >> 17;
	number ^= number << 5;

	random_state = number;

	return number;
}


U64 get_random_U64_numbers()
{
	// define 4 random numbers
	U64 n1, n2, n3, n4;

	// init random numbers slicing 16 bits from MS1B side
	n1 = (U64)(get_random_U32_number()) & 0xFFFF;
	n2 = (U64)(get_random_U32_number()) & 0xFFFF;
	n3 = (U64)(get_random_U32_number()) & 0xFFFF;
	n4 = (U64)(get_random_U32_number()) & 0xFFFF;

	// return random number
	return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 generate_magic_number()
{
	return get_random_U64_numbers() & get_random_U64_numbers() & get_random_U64_numbers();
}






