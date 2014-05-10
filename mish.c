/////////////////////////////////////////////////
//
// author: Alejandro Mancillas
//
// mish.c
//
// main contained here
// int main( int argc, char *argv[]);
//
// prototype for internal mish commands
// int mish_command_name( int argc, char * argv [] );
//
////////////////////////////////////////////////

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_ERR_LENGTH 80
#define MAX_TXT_LENGTH 600
#define MAX_TOKENS 80;

struct ParsingResult{
	int numArgs;
	char type;
	char *strs[];
};

////////////////////////////////////////////////
// parsing - start
////////////////////////////////////////////////
void parseInput(char txtbuf[], struct ParsingResult *pr) 
{
	int token_num = 0;
	
	char *token = strtok(txtbuf, " ");
	
	while( token != NULL )
	{
		//printf( "from parse[%d] [%s]\n", token_num, token );
		pr->strs[token_num] = token;
		token = strtok(NULL, " ");
		token_num++;
	}
	pr->numArgs = token_num;
}
////////////////////////////////////////////////
// parsing - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// main - start
////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
	char txtbuf[MAX_TXT_LENGTH];
	int current_number = 1; //starting number seems to be one
	
	int strf;
	
	int child_id;
	
	struct ParsingResult *pr = (struct ParsingResult*)malloc( sizeof(struct ParsingResult ) ); 
	pr->numArgs = 0;
	pr->type = (int)"x";
	
	while( true )
	{
		printf("mish[%d]> ", current_number);
		//endless loop unless broken by a break
		
		if( fgets(txtbuf,MAX_TXT_LENGTH,stdin) == NULL ) {
			break;
		}

		//
		// Locate the newline character in the buffer,
		// and replace it with a NUL character
		//
		strf = 0;
		while( txtbuf[strf] != '\0' && txtbuf[strf] != '\n' ) {
			++strf;
		}

		if( txtbuf[strf] == '\n' ) {
			txtbuf[strf] = '\0';
		}
		
		parseInput(txtbuf,pr);
		current_number++;
		/*int q = 0;
		while(q < pr->numArgs)
		{
			printf( "from main[%d] [%s]\n",q,pr->strs[q]);
			//printf( "from parse[%d] [%s]\n", token_num, token );
			q++;
		}*/

	}
	
	return 0;
}
////////////////////////////////////////////////
// main - end
////////////////////////////////////////////////