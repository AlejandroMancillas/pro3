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

////////////////////////////////////////////////
// parsing - start
////////////////////////////////////////////////

////////////
//	parseInput
//	arguments:
//		txtbuf	the inputed string
//		parres	ParsingResult struct for the managing on the
//				parsed information
//	
//	partitions the inputed string into the string array inside
//	parres
//	also updates the type of command, the number of arguments,
//	and the internal command error if needed
////////////
void parseInput(char txtbuf[], struct ParsingResult *parres) 
{
	int token_num = 0;	
	char *token = strtok(txtbuf, " ");
	
	while( token != NULL )
	{
		// takes the input and partitions it based on space chars
		
		// commented out printf right below tests to see if the string is
		// being parsed okay	-- success
		//printf( "(test output) from parse[%d] [%s]\n", token_num, token );
		
		//store token in struct
		parres->strs[token_num] = token;
		
		//get next token and increment
		token = strtok(NULL, " ");
		token_num++;
	}
	//managing what kind of token I got out of it
	// 0 for nothing at all
	// 1 for internal function
	// 2 for everything else
	// 3 display usage info
	parres->numArgs = token_num;
	
	if(token_num == 0)
	{
		parres->type = 0;
	}
	else if(strcmp(parres->strs[0],"help")==0 ||
			strcmp(parres->strs[0],"history")==0 ||
			strcmp(parres->strs[0],"quit")==0 )
	{
		//possibly internal
		if(token_num > 1)
		{//failed internal
			parres->type = 3;
			strcpy(parres->display, "	usage: ");
			strcat(parres->display,parres->strs[0]);
		}
		else
		{//success internal
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
	{//just about everything else
		parres->type = 2;
	}
}
////////////////////////////////////////////////
// parsing - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// history - start
////////////////////////////////////////////////
//
//	addToHistory
//	args
//		str		desired entry
//		history	the history struct in which to store
//				it
//		
//	takes the desired string and adds it to the string
//	array in the struct, should the array already be
//	full it will take the earliest out
//
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
//
//	showHistory
//	args
//		history	-the history struct that holds the
//				strings we want to output
//
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
//
//	showHelp	displays help information
//
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
// help - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// external - start
////////////////////////////////////////////////
//failed checking algorith, not part of the submission
// not deleted because it will be worked on even after submission
int checkExist(char *str)
{
	// commented out to further show that it is not part
	// of the project 3 submittion
	
	/*printf("flag 1[%s]\n",str);
    int exists = 0;
	char *line = (char*) malloc(strlen(str));
	strcpy(line,str);
	printf("flag 1[%s][%s]\n",str,line);
	char* command = (char*) malloc(strlen(line) + 30);
	sprintf(command, "bash -cli 'command -v %s'", line);
	printf("command: %s\n",command);
    //first we try to execute the command as a dummy user.
    exists |= system(command) == 0;

    return exists;*/
	return 0;
}

void externalCommands(struct ParsingResult *parres,int verbose_on)
{
	pid_t id, my_id;	//id management
	int status;
	char *message;		//forking identification
	
	my_id = getpid();	//obtain id
	
	id = fork();		//create child
	
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
		if(verbose_on) // ex.	wait for pid 1244: ls
			printf( "	wait for pid %d: %s\n",my_id,parres->strs[0]);
		
		char *line = strdup(parres->strs[0]);
		if(parres->numArgs > 1)//	constructs the bash line argument
		{//							ex. echo 'Hello World'
			line = strcat(line," '");
			int i = 1;
			while(i < parres->numArgs)
			{
				line = strcat(line,parres->strs[i]);
				if(i < parres->numArgs-1)
					line = strcat(line," ");
				i++;
			}
			line = strcat(line,"'");
		}
		
		//previous bash command method
		/*char *name[] = {
			"/bin/bash",
			"-c",
			line,
			NULL
		};*/
		if(verbose_on)//ex. 	execvp: ls
			printf("	execvp: %s\n",parres->strs[0]);
			
		//execvp(name[0], name);
		//printf("system %d\n",system(line));
		int temp = system(line);
		if(verbose_on)//ex. command status: 2
			printf( "command status %d\n", temp );
			
		_exit( EXIT_SUCCESS );
	}
	
	id = wait( &status );
	
	if( id < 0)
	{
		perror( "error while waiting" );
	}
}
////////////////////////////////////////////////
// external - end
////////////////////////////////////////////////

////////////////////////////////////////////////
// main - start
////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
	
	int verbose_on = 0; 
	char txtbuf[MAX_TXT_LENGTH];
	int current_number = 1; //starting number seems to be one
	char copy[MAX_TXT_LENGTH];
	int continue_loop = 1;
	
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
		
		//originally located above with the rest, but due to the nature
		//of the command it must be placed up here before it is it used
		if(parres->type == 1 && strcmp(parres->strs[0],"verbose")==0)
		{
			verbose_on = (strcmp(parres->strs[1],"on")==0);
		}
		
		if(verbose_on)
		{//and so verbose said, "Let there be cake! -I mean info, let there be info"
			printf("	command: %s\n\n	input command tokens:\n",copy);
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
		if(parres->type == 1)//internal commands
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
			/*else if(strcmp(parres->strs[0],"verbose")==0)
			{
				//filtered everything out from the parse function,
				//only on and off are possible
				verbose_on = (strcmp(parres->strs[1],"on")==0);
			}*/
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
			externalCommands(parres,verbose_on);
			current_number++;
		}
	}
	
	return 0;
}
////////////////////////////////////////////////
// main - end
////////////////////////////////////////////////