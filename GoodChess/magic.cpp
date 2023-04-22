#include "magic.h"
#include "attacks.h"
#include "bitboard.h"
#include <cstring>


const U64 rook_magics[64] =
{
0x8a80104000800020ULL,0x0140002000100040ULL,0x02801880a0017001ULL,0x0100081001000420ULL,
0x0200020010080420ULL,0x03001c0002010008ULL,0x8480008002000100ULL,0x2080088004402900ULL,
0x0000800098204000ULL,0x2024401000200040ULL,0x0100802000801000ULL,0x0120800800801000ULL,
0x0208808088000400ULL,0x0002802200800400ULL,0x2200800100020080ULL,0x0801000060821100ULL,
0x0080044006422000ULL,0x0100808020004000ULL,0x12108a0010204200ULL,0x0140848010000802ULL,
0x0481828014002800ULL,0x8094004002004100ULL,0x4010040010010802ULL,0x0000020008806104ULL,
0x0100400080208000ULL,0x2040002120081000ULL,0x0021200680100081ULL,0x0020100080080080ULL,
0x0002000a00200410ULL,0x0000020080800400ULL,0x0080088400100102ULL,0x0080004600042881ULL,
0x4040008040800020ULL,0x0440003000200801ULL,0x0004200011004500ULL,0x0188020010100100ULL,
0x0014800401802800ULL,0x2080040080800200ULL,0x0124080204001001ULL,0x0200046502000484ULL,
0x0480400080088020ULL,0x1000422010034000ULL,0x0030200100110040ULL,0x0000100021010009ULL,
0x2002080100110004ULL,0x0202008004008002ULL,0x0020020004010100ULL,0x2048440040820001ULL,
0x0101002200408200ULL,0x0040802000401080ULL,0x4008142004410100ULL,0x02060820c0120200ULL,
0x0001001004080100ULL,0x020c020080040080ULL,0x2935610830022400ULL,0x0044440041009200ULL,
0x0280001040802101ULL,0x2100190040002085ULL,0x80c0084100102001ULL,0x4024081001000421ULL,
0x00020030a0244872ULL,0x0012001008414402ULL,0x02006104900a0804ULL,0x0001004081002402ULL
};

const U64 bishop_magics[64] =
{
0x0040040844404084ULL,0x002004208a004208ULL,0x0010190041080202ULL,0x0108060845042010ULL,
0x0581104180800210ULL,0x2112080446200010ULL,0x1080820820060210ULL,0x03c0808410220200ULL,
0x0004050404440404ULL,0x0000021001420088ULL,0x24d0080801082102ULL,0x0001020a0a020400ULL,
0x0000040308200402ULL,0x0004011002100800ULL,0x0401484104104005ULL,0x0801010402020200ULL,
0x00400210c3880100ULL,0x0404022024108200ULL,0x0810018200204102ULL,0x0004002801a02003ULL,
0x0085040820080400ULL,0x810102c808880400ULL,0x000e900410884800ULL,0x8002020480840102ULL,
0x0220200865090201ULL,0x2010100a02021202ULL,0x0152048408022401ULL,0x0020080002081110ULL,
0x4001001021004000ULL,0x800040400a011002ULL,0x00e4004081011002ULL,0x001c004001012080ULL,
0x8004200962a00220ULL,0x8422100208500202ULL,0x2000402200300c08ULL,0x8646020080080080ULL,
0x80020a0200100808ULL,0x2010004880111000ULL,0x623000a080011400ULL,0x42008c0340209202ULL,
0x0209188240001000ULL,0x400408a884001800ULL,0x00110400a6080400ULL,0x1840060a44020800ULL,
0x0090080104000041ULL,0x0201011000808101ULL,0x1a2208080504f080ULL,0x8012020600211212ULL,
0x0500861011240000ULL,0x0180806108200800ULL,0x4000020e01040044ULL,0x300000261044000aULL,
0x0802241102020002ULL,0x0020906061210001ULL,0x5a84841004010310ULL,0x0004010801011c04ULL,
0x000a010109502200ULL,0x0000004a02012000ULL,0x500201010098b028ULL,0x8040002811040900ULL,
0x0028000010020204ULL,0x06000020202d0240ULL,0x8918844842082200ULL,0x4010011029020020ULL
};



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


U64 find_magic_number(int square, int bits, PieceType pt)
{

	U64 occupancies[4096];
	U64 attacks[4096];
	U64 used_attacks[4096];
	U64 attack_mask = (pt == BISHOP) ? mask_bishop_occupancy(square) : mask_rook_occupancy(square);

	int occupancy_indices = 1 << bits;

	for (int index = 0; index < occupancy_indices; ++index)
	{
		occupancies[index] = set_occupancy(index, bits, attack_mask);
		attacks[index] = (pt == BISHOP) ? mask_bishop_attacks(square, occupancies[index]) : mask_rook_attacks(square, occupancies[index]);
	}

	for (int random_count = 0; random_count < 1000000000; ++random_count)
	{
		U64 magic_number = generate_magic_number();
		if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

		memset(used_attacks, 0ULL, sizeof(used_attacks));

		int index, fail;

		for (index = 0, fail = 0; !fail && index < occupancy_indices; ++index)
		{
			int magic_index = (int)( (occupancies[index] * magic_number) >> (64 - bits) );

			if (used_attacks[magic_index] == 0ULL)
			{
				used_attacks[magic_index] = attacks[index];
			}
			else if (used_attacks[magic_index] != attacks[index])
			{
				fail = 1;
			}
		}
		if (!fail) return magic_number;
	}
	//printf(" Magic number fail.\n");
	return 0ULL;
}


void init_magic_numbers()
{
	//for (int square = 0; square < 64; ++square)
	//{
	//	std::cout << "0x" << std::setfill('0') << std::setw(sizeof(U64) * 2)
	//		<< std::hex << find_magic_number(square, rook_relevant_occupancy[square], ROOK) << "ULL,";
	//	if (square % 4 == 3) std::cout << std::endl;
	//	//printf(" 0x%llxULL, \n", find_magic_number(square, rook_relevant_occupancy[square], ROOK));
	//}
	//for (int square = 0; square < 64; ++square)
	//{
	//	std::cout << "0x" << std::setfill('0') << std::setw(sizeof(U64) * 2)
	//		<< std::hex << find_magic_number(square, bishop_relevant_occupancy[square], BISHOP) << "ULL,";
	//	if (square % 4 == 3) std::cout << std::endl;
	//	//printf(" 0x%llxULL, \n", find_magic_number(square, rook_relevant_occupancy[square], ROOK));
	//}
}


