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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ERR_LENGTH 80
#define MAX_TXT_LENGTH 600
#define MAX_TOKENS 80
#define HISTORY_MAX 10

struct ParsingResult{
	int numArgs;
	int type;
	char display[MAX_ERR_LENGTH];
	char *strs[MAX_TOKENS];
};
struct History{
	int hnum;
	char *hist[HISTORY_MAX];
};

////////////////////////////////////////////////
// parsing - start
////////////////////////////////////////////////
void parseInput(char txtbuf[], struct ParsingResult *parres) 
{
	int token_num = 0;
	
	char *token = strtok(txtbuf, " ");
	
	while( token != NULL )
	{
		// commented out printf right below tests to see if the string is
		// being parsed okay
		//printf( "(test output) from parse[%d] [%s]\n", token_num, token );
		
		//store token in struct
		parres->strs[token_num] = token;
		
		//get next token and increment
		token = strtok(NULL, " ");
		token_num++;
	}
	//managin what kind of token I got out of it
	// 0 for nothing at all
	// 1 for internal function
	// 2 for everything else
	// 3 display usage info
	parres->numArgs = token_num;
	//fix in a sec
	
	if(token_num == 0)
	{
		parres->type = 0;
	}
	else if(strcmp(parres->strs[0],"help")==0 ||
			strcmp(parres->strs[0],"history")==0 ||
			strcmp(parres->strs[0],"quit")==0 )
	{
		//manage 3 of the 4 internal commands through here
		if(token_num > 1)
		{
			parres->type = 3;
			strcpy(parres->display, "	usage: ");
			strcat(parres->display,parres->strs[0]);
		}
		else
		{
			parres->type = 1;
		}
	}
	else if(strcmp(parres->strs[0],"verbose")==0)
	{
		if(token_num > 2)
		{
			parres->type = 3;
			strcpy(parres->display, "	usage: verbose [on|off]");
		}
		else if(strcmp(parres->strs[1],"on")==0 ||
			strcmp(parres->strs[1],"off")==0)
		{
			parres->type = 1;
		}
		else
		{
			parres->type = 3;
			strcpy(parres->display, "	usage: verbose [on|off]");
		}
	}
	else
	{
		parres->type = 2;
	}
}
////////////////////////////////////////////////
// parsing - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// history - start
////////////////////////////////////////////////
void addToHistory(char str[], struct History *history) 
{
	if(history->hnum < HISTORY_MAX)
	{//max has not been reached
		history->hist[history->hnum] = strdup(str);
		history->hnum++;
	}
	else
	{//now it gets interesting
		int r = 0;
		char *temp = history->hist[0];
		while(r < HISTORY_MAX-1)
		{
			history->hist[r] = history->hist[r+1];
			r++;
		}
		history->hist[HISTORY_MAX-1] = strdup(str);
		free(temp);
	}
}
void showHistory(struct History *history)
{
	printf("	Displaying history:\n");
	int t = 0;
	while(t < history->hnum)
	{
		printf("		%d: %s\n",t+1,history->hist[t]);
		t++;
	}
}
////////////////////////////////////////////////
// history - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// help - start
////////////////////////////////////////////////
void showHelp()
{
	printf("	Displaying Help:\n");
	printf("		verbose:	usage: verbose [on|off]\n");
	printf("			When on, the program displays additional.\n");
	printf("			information.\n");
	printf("		help:		usage: help\n");
	printf("			displays information on internal commands.\n");
	printf("		history:	usage: history\n");
	printf("			print a list of commands executed so far,.\n");
	printf("			including their arguments.\n");
	printf("		quit:		usage: quit\n");
	printf("			terminates the mish shell.\n");
}
////////////////////////////////////////////////
// help - start
////////////////////////////////////////////////

////////////////////////////////////////////////
// main - start
////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
	
	int verbose_on = 1; 
	char txtbuf[MAX_TXT_LENGTH];
	int current_number = 1; //starting number seems to be one
	char copy[MAX_TXT_LENGTH];
	char *message;
	int continue_loop = 1;
	pid_t id, my_id;
	int status;
	
	my_id = getpid();
	
	struct ParsingResult *parres = (struct ParsingResult*)malloc( sizeof(struct ParsingResult ) ); 
	parres->numArgs = 0;
	parres->type = 0;
	struct History *history = (struct History*)malloc( sizeof(struct History ) ); 
	history->hnum = 0;
	
	while( continue_loop )
	{
		printf("mish[%d]> ", current_number);
		if( fgets(txtbuf,MAX_TXT_LENGTH,stdin) == NULL ) {
			break;
		}
		// Locate the newline character in the buffer,
		// and replace it with a NUL character
		int i = 0;
		while( txtbuf[i] != '\0' && txtbuf[i] != '\n' ) {
			i++;
		}
		if( txtbuf[i] == '\n' ) {
			txtbuf[i] = '\0';
		}
		
		//since txtbuf gets altered after this point, lets take the copy now
		strcpy(copy,txtbuf);
		
		parseInput(txtbuf,parres);
		if(verbose_on)
		{//and so verbose said, "Let there be cake! -I mean info, let there be info"
			printf("	command: %s\n\n",copy);
			int q = 0;
			while(q < parres->numArgs)
			{
				printf( "	%d: %s\n",q,parres->strs[q]);
				q++;
			}
			
		}
		if(parres->type != 0)//command entered, lets archive it
		{
			addToHistory(copy,history);
		}
		
		//think of parres->type as an enum
		if(parres->type == 0)
		{
			id = fork();
			switch( id )
			{
				case -1: //the fork has failed, Jim
					perror( "fork" );
					exit( EXIT_FAILURE );//RIP program
				case 0: //this is the child class process
					message = "child";
					my_id = getpid();
					break;
				default: //parent process
					message = "parent";
					break;
			}
			
			if(strcmp(message,"child")==0)
			{
				char *name[] = {
					"/bin/bash",
					"-c",
					"echo 'Hello World(echo)'",
					//"date",
					//"which grep",
					NULL
				};
				execvp(name[0], name);
			}
			
			id = wait( &status );
			/*if( id < 0)
			{
				perror( "wait" );
			}
			else
			{
				printf( "Parent: child %d terminated, status %d\n",id, status );
			}*/
		}
		if(parres->type == 1)
		{
			if(strcmp(parres->strs[0],"help")==0)
			{
				showHelp();
			}
			else if(strcmp(parres->strs[0],"quit")==0)
			{
				continue_loop = 0;
			}
			else if(strcmp(parres->strs[0],"history")==0)
			{
				showHistory(history);
			}
			else if(strcmp(parres->strs[0],"verbose")==0)
			{
				//filtered everything out from the parse function,
				//only on and off are possible
				verbose_on = (strcmp(parres->strs[1],"on")==0);
			}
			current_number++;
		}
		else if(parres->type == 3)
		{
			//an error has occurred in the internal functions,
			//we must inform the masses
			printf("%s\n",parres->display);
		}
		else if(parres->type == 2)
		{
			/*id = fork();
			switch( id )
			{
				case -1: //the fork has failed, Jim
					perror( "fork" );
					exit( EXIT_FAILURE );//RIP program
				case 0: //this is the child class process
					message = "child";
					my_id = getpid();
					break;
				default: //parent process
					message = "parent";
					break;
			}
			
			if(strcmp(message,"child")==0)
			{
				char str[];
				char *name[] = {
					"/bin/bash",
					"-c",
					"echo 'Hello World(echo)'",
					NULL
				};
				execvp(name[0], name);
			}
			
			id = wait( &status );*/
			/*if( id < 0)
			{
				perror( "wait" );
			}
			else
			{
				printf( "Parent: child %d terminated, status %d\n",id, status );
			}*/
			
			current_number++;
		}
	}
	
	return 0;
}
////////////////////////////////////////////////
// main - end
////////////////////////////////////////////////