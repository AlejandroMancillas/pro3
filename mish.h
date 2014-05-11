/////////////////////////////////////////////////
//
// author: Alejandro Mancillas
//
// mish.h
//
////////////////////////////////////////////////

#define MAX_ERR_LENGTH 80
#define MAX_TXT_LENGTH 600
#define MAX_TOKENS 80
#define HISTORY_MAX 10

// Parsing result stores the content parsed out
// of user input.
//		numArgs keeps track of how many arguments
//					were taken in
//		type	it's an enum:
//					0: nothing / empty
//					1: internal command
//					2: external command
//					3: internal error
//		display	for when type is 3
//		strs	the parsed strings
struct ParsingResult{
	int numArgs;
	int type;
	char display[MAX_ERR_LENGTH];
	char *strs[MAX_TOKENS];
};
// the history of the program
//		hnum	containts the number of history entries
//		hist	the actual entries
struct History{
	int hnum;
	char *hist[HISTORY_MAX];
};

void parseInput(char txtbuf[], struct ParsingResult *parres);
void addToHistory(char str[], struct History *history);
void showHistory(struct History *history);
void showHelp();
int checkExist(char *str);
void externalCommands(struct ParsingResult *parres,int verbose_on);