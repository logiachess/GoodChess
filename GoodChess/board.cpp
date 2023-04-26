#include "board.h"
#include "bitboard.h"
#include <cstring>
#include <map>
#include "misc.h"



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


std::map<char, int> char_pieces = {
	{'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK}, {'p', BP},
	{'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK}, {'.', NO_SQUARE}
};

const char ascii_pieces[] = "PNBRQKpnbrqk. ";

int side = -1;
int enpassant = NO_SQUARE;
int castle;
Bitboard bitboards[12] = {};
Bitboard occupancies[3] = {};



void print_board()
{
	printf("\n");
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			if (!file) printf(" %d  ", 8 - rank);
			int piece = BLANK_PIECE;

			for (int bb_piece = WP; bb_piece <= BK; ++bb_piece)
			{
				if (get_bit(bitboards[bb_piece], square))
				{
					piece = bb_piece;
				}
			}

			printf(" %c", ascii_pieces[piece]);
		}
		printf("\n");
	}

	// Print files
	printf("\n     a b c d e f g h \n\n");
	printf("     Side:   %s\n", (!side) ? "white" : "black");
	printf("     Enpas: %s\n", (enpassant != NO_SQUARE) ? sq_to_coord[enpassant] : "None");
	printf("     Castle: %c%c%c%c\n\n", (castle & WKCA) ? 'K' : '-', (castle & WQCA) ? 'Q' : '-', (castle & BKCA) ? 'k' : '-', (castle & BQCA) ? 'q' : '-');
}


void parse_fen(const std::string& command)
{
	memset(bitboards, 0ULL, sizeof(bitboards));

	memset(occupancies, 0ULL, sizeof(occupancies));

	enpassant = NO_SQUARE;
	castle = 0;

	std::vector<std::string> tokens = split_command(command);
	const std::string pos_string = tokens.at(0);
	const std::string turn = tokens.at(1);
	const std::string castle_perm = tokens.at(2);
	const std::string ep_square = tokens.at(3);
	std::string fifty_move;
	std::string HisPly;

	//Keep fifty move and Hisply arguments optional
	if (tokens.size() >= 5) {
		fifty_move = tokens.at(4);
		if (tokens.size() >= 6) {
			HisPly = tokens.at(5);
			HisPly = tokens.at(5);
		}
	}

	int fen_counter = 0;
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			const char current_char = pos_string[fen_counter];

			if ((current_char >= 'a' && current_char <= 'z') || (current_char >= 'A' && current_char <= 'Z')) {
				// init piece type
				const int piece = char_pieces[current_char];

				set_bit(bitboards[piece], square);
				++fen_counter;
			}

			// match empty square numbers within FEN string
			if (current_char >= '0' && current_char <= '9') {
				// init offset (convert char 0 to int 0)
				const int offset = current_char - '1';
				file += offset;
				// increment pointer to FEN string
				++fen_counter;
			}

			// match rank separator
			if (pos_string[fen_counter] == '/')
				// increment pointer to FEN string
				++fen_counter;


		}
	}

	//Parse turn
	side = (turn == "w") ? WHITE : BLACK;

	//Parse castling rights
	for (const char c : castle_perm) {
		switch (c) {
		case 'K':
			(castle) |= WKCA;
			break;
		case 'Q':
			(castle) |= WQCA;
			break;
		case 'k':
			(castle) |= BKCA;
			break;
		case 'q':
			(castle) |= BQCA;
			break;
		case '-':
			break;
		}
	}

	// parse enpassant square
	if (ep_square != "-" && ep_square.size() == 2) {
		// parse enpassant file & rank
		const int file = ep_square[0] - 'a';
		const int rank = 8 - (ep_square[1] - '0');

		// init enpassant square
		enpassant = rank * 8 + file;
	}
	// no enpassant square
	else
		enpassant = NO_SQUARE;

	// loop over white pieces bitboards
	for (int piece = WP; piece <= WK; ++piece)
		// populate white occupancy bitboard
		occupancies[WHITE] |= bitboards[piece];

	// loop over black pieces bitboards
	for (int piece = BP; piece <= BK; ++piece)
		// populate white occupancy bitboard
		occupancies[BLACK] |= bitboards[piece];

	// init all occupancies
	occupancies[BOTH] |= occupancies[WHITE];
	occupancies[BOTH] |= occupancies[BLACK];


}



char* Pr_move(const int move)
{
	static char MvStr[6];
	sprintf(MvStr, "%s%s%c", (sq_to_coord[get_move_from(move)]), (sq_to_coord[get_move_to(move)]), (ascii_pieces[get_move_promotion(move)]));
	return MvStr; // Return pointer to array
}


void print_moves_list(const Moves_list* moves_list)
{
	printf("\n    move    piece   capture   double    enpass    castling\n\n");
	for (int move_count = 0; move_count < moves_list->count; ++move_count)
	{
		int move = moves_list->moves[move_count];
		printf("    %s   %c       %d         %d         %d         %d\n", Pr_move(move), ascii_pieces[get_move_piece(move)], get_move_capture(move),
			get_move_double(move),
			get_move_enpas(move),
			get_move_castle(move));
	}
	printf("\n\n    Total number of moves: %d\n\n", moves_list->count);
}


















