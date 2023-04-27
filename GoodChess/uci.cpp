#include "uci.h"
#include "movegen.h"
#include "perft.h"
#include "search.h"
#include <iostream>

#include "evaluate.h"


enum Uci_commands
{
	U_unrecognized, U_go, U_ucinewgame, U_stop, U_perft, U_quit, U_position, U_uci, U_isready, U_display
};

static std::map<std::string, Uci_commands> map_Uci_commands =
{
	{"go", U_go}, {"ucinewgame", U_ucinewgame}, {"stop", U_stop}, {"perft", U_perft}, 
	{"quit", U_quit}, {"position", U_position}, {"uci", U_uci}, {"isready", U_isready},
	{"d", U_display}
};

//convert a move to coordinate notation to internal notation
static int ParseMove(const std::string& move_string) {

	// parse source square
	const int from = (move_string[0] - 'a') +  (8 - (move_string[1] - '0')  ) * 8;

	// parse target square
	const int to = (move_string[2] - 'a') +  (8 - (move_string[3] - '0')  ) * 8;

	Moves_list list[1];
	generate_moves(list);

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum)
	{
		int move = list->moves[MoveNum].move;
		if (from == get_move_from(move) && to == get_move_to(move))
		{
			const int promotion_piece = get_move_promotion(move);
			if (promotion_piece == NO_PIECE) return move;
			switch (promotion_piece)
			{
			case (WQ):
			case (BQ):
				if (move_string[4] == 'q') return move;
				break;
			case (WR):
			case (BR):
				if (move_string[4] == 'r') return move;
				break;
			case (WB):
			case (BB):
				if (move_string[4] == 'b') return move;
				break;
			case (WN):
			case (BN):
				if (move_string[4] == 'n') return move;
				break;
			}
			continue;
		}

	}

	// return illegal move
	return FALSE;
}


// parses the moves part of a fen string and plays all the moves included
static void parse_moves(const std::string moves)
{
	std::vector<std::string> move_tokens = split_command(moves);
	// loop over moves within a move string
	for (size_t i = 0; i < move_tokens.size(); ++i) {
		// make move on the chess board
		make_move(ParseMove(move_tokens[i]));
	}
}

// parse UCI "position" command
static void ParsePosition(const std::string& command) {

	// parse UCI "startpos" command
	if (command.find("startpos") != std::string::npos) {
		// init chess board with start position
		parse_fen(start_position);
	}

	// parse UCI "fen" command
	else {

		// if a "fen" command is available within command string
		if (command.find("fen") != std::string::npos) {
			// init chess board with position from FEN string
			parse_fen(command.substr(command.find("fen") + 4, std::string::npos));
		}
		else {
			// init chess board with start position
			parse_fen(start_position);
		}

	}

	// if there are moves to be played in the fen play them
	if (command.find("moves") != std::string::npos) {
		int string_start = command.find("moves") + 6;
		std::string moves_substr = command.substr(string_start, std::string::npos);
		parse_moves(moves_substr);
	}
}

static void ParseGo(const std::string& line)
{
	int search_depth = MAX_DEPTH, movetime = -1, movestogo = 25;
	long long nodes = -1;
	int time = -1; int inc = 0;
	bool movestogoset = false;
	Search_info info[1];

	std::vector<std::string> tokens = split_command(line);

	//loop over all the tokens and parse the commands
	for (size_t i = 1; i < tokens.size(); ++i) {

		if (tokens.at(1) == "infinite") {
			continue;
		}

		if (tokens.at(i) == "winc" && side == WHITE) {
			inc = std::stoi(tokens[i + 1]);
			continue;
		}

		if (tokens.at(i) == "binc" && side == BLACK) {
			inc = std::stoi(tokens[i + 1]);
			continue;
		}

		if (tokens.at(i) == "wtime" && side == WHITE) {
			time = std::stoi(tokens[i + 1]);
			info->timeset = true;
			continue;
		}
		if (tokens.at(i) == "btime" && side == BLACK) {
			time = std::stoi(tokens[i + 1]);
			info->timeset = true;
			continue;
		}

		if (tokens.at(i) == "movestogo") {
			movestogo = std::stoi(tokens[i + 1]);
			movestogoset = true;
			continue;
		}

		if (tokens.at(i) == "movetime") {
			movetime = std::stoi(tokens[i + 1]);
			time = movetime;
			info->timeset = true;
			continue;
		}

		if (tokens.at(i) == "depth") {
			search_depth = std::stoi(tokens[i + 1]);
			continue;
		}

		if (tokens.at(i) == "nodes") {
			nodes = std::stoi(tokens[i + 1]);
			continue;
		}
	}
	info->starttime = get_time_ms();
	info->S_depth = search_depth;

	static const int safety_overhead = 50;

	// calculate
	//calculate time allocation for the move
	if (info->timeset and movetime != -1) {
		time -= safety_overhead;
		info->stoptime = info->starttime + time + inc;
		info->optstoptime = info->starttime + time + inc;
	}
	else if (info->timeset && movestogoset)
	{
		time -= safety_overhead;
		int time_slot = time / info->movestogo;
		int basetime = (time_slot);
		info->stoptime = info->starttime + basetime;
		info->optstoptime = info->starttime + basetime;
	}
	else if (info->timeset)
	{
		time -= safety_overhead;
		int time_slot = time / movestogo + inc / 2;
		int basetime = (time_slot);
		//optime is the time we use to stop if we just cleared a depth
		int optime = basetime * 0.6;
		//maxtime is the absolute maximum time we can spend on a search
		int maxtime = std::min(time, basetime * 2);
		info->stoptime = info->starttime + maxtime;
		info->optstoptime = info->starttime + optime;
	}

	std::cout << "info ";
	std::cout << "time: " << time << " ";
	std::cout << "start: " << info->starttime << " ";
	std::cout << "stop: " << info->stoptime << " ";
	std::cout << "depth: " << info->S_depth << " \n";

	search_position(info);
}





void Uci_Loop()
{
	bool parsed_position = FALSE;



	// Start uci
	printf("id name %s\n", NAME);
	printf("id author Nathanael Lu\n");
	//printf("option name Hash type spin default 512 min 4 max %d\n", MAX_HASH);
	//printf("option name OwnBook type check default false\n");
	//printf("option name Threads type spin default 1 min 1 max %d\n", MAXTHREADS);
	printf("uciok\n");

	while (TRUE)
	{

		std::string input;


		// get user / GUI input
		if (!std::getline(std::cin, input))
		{
			// continue the loop
			break;
		}

		// make sure input is available
		if (!input.length())
		{
			// continue the loop
			continue;
		}

		//Split the string into tokens to make it easier to work with
		std::vector<std::string> tokens = split_command(input);



		switch (map_Uci_commands[tokens[0]])
		{
		case (U_quit):
		{

			goto exit_loop;
			break;
		}

		case (U_go):
		{

			if (!parsed_position) // call parse position function
			{
				ParsePosition("position startpos");
			}
			ParseGo(input);
			break;
		}

		case (U_position):
		{
			// call parse position function
			ParsePosition(input);
			parsed_position = true;
			break;
		}

		case (U_isready):
		{

			std::cout << "readyok\n";
			break;
		}

		case (U_ucinewgame):
		{
			ParsePosition("position startpos\n");
			break;
		}
		case (U_perft):
			{
			const int p_depth = std::stoi(tokens[1]);
			if (!parsed_position) // call parse position function
			{
				ParsePosition("position startpos");
			}
			perft_test(p_depth);
			break;
			}
		case (U_display):
		{
			print_board();
			break;
		}
		case (U_uci):
		{
			printf("id name %s\n", NAME);
			printf("id author Nathanael Lu\n");
			printf("uciok\n");
			break;
		}
		case (U_unrecognized):
		{
			printf("Unknown command\n");
			break;
		}
		// End switch
		}


	}
	exit_loop:
		return;
}















