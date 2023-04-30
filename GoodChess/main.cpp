#include <stdio.h>
#include "init.h"
#include "board.h"
#include "search.h"
#include "uci.h"


int main()
{
	InitAll();

	BOARD pos[1];
	Search_info info[1];
	info->quit = FALSE;


	//HashTable->pTable = NULL;
	//InitHashTable(HashTable, 512);

	Uci_Loop(pos, info);


	/*free(HashTable->pTable);*/
	return 0;
}




















