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

//
//int side = -1;
//int enpassant = NO_SQUARE;
//int castle;
//Bitboard bitboards[12] = {};
//Bitboard occupancies[3] = {};
//int hisPly; // total game
//int ply; // search
//int fiftymove;



void print_board(BOARD *pos)
{
	printf("\n\n");
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			if (!file) printf(" %d  ", 8 - rank);
			int piece = BLANK_PIECE;

			for (int bb_piece = WP; bb_piece <= BK; ++bb_piece)
			{
				if (get_bit(pos->bitboards[bb_piece], square))
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
	printf("     Side:   %s\n", (!pos->side) ? "white" : "black");
	printf("     Enpas: %s\n", (pos->enpassant != NO_SQUARE) ? sq_to_coord[pos->enpassant] : "None");
	printf("     Castle: %c%c%c%c\n\n", (pos->castle & WKCA) ? 'K' : '-', (pos->castle & WQCA) ? 'Q' : '-', (pos->castle & BKCA) ? 'k' : '-', (pos->castle & BQCA) ? 'q' : '-');
}


void parse_fen(const std::string& command, BOARD * pos)
{
	memset(pos->bitboards, 0ULL, SIZEOF_BITBOARD);

	memset(pos->occupancies, 0ULL, SIZEOF_OCCUPANCIES);

	pos->enpassant = NO_SQUARE;
	pos->castle = 0;

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

				set_bit(pos->bitboards[piece], square);
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
	pos->side = (turn == "w") ? WHITE : BLACK;

	//Parse castling rights
	for (const char c : castle_perm) {
		switch (c) {
		case 'K':
			(pos->castle) |= WKCA;
			break;
		case 'Q':
			(pos->castle) |= WQCA;
			break;
		case 'k':
			(pos->castle) |= BKCA;
			break;
		case 'q':
			(pos->castle) |= BQCA;
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
		pos->enpassant = rank * 8 + file;
	}
	// no enpassant square
	else
		pos->enpassant = NO_SQUARE;

	//Read fifty moves counter
	if (!fifty_move.empty()) {
		pos->fiftymove = std::stoi(fifty_move);
	}
	//Read Hisply moves counter
	if (!HisPly.empty()) {

		pos->hisPly = std::stoi(HisPly);

	}


	// loop over white pieces bitboards
	for (int piece = WP; piece <= WK; ++piece)
		// populate white occupancy bitboard
		pos->occupancies[WHITE] |= pos->bitboards[piece];

	// loop over black pieces bitboards
	for (int piece = BP; piece <= BK; ++piece)
		// populate white occupancy bitboard
		pos->occupancies[BLACK] |= pos->bitboards[piece];


	// init all occupancies
	pos->occupancies[BOTH] |= pos->occupancies[WHITE];
	pos->occupancies[BOTH] |= pos->occupancies[BLACK];
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
		int move = moves_list->moves[move_count].move;
		printf("    %s   %c       %d         %d         %d         %d\n", Pr_move(move), ascii_pieces[get_move_piece(move)], get_move_capture(move),
			get_move_double(move),
			get_move_enpas(move),
			get_move_castle(move));
	}
	printf("\n\n    Total number of moves: %d\n\n", moves_list->count);
}


















