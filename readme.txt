/////////////////////////////////////////////////
//
// author: Alejandro Mancillas
//
////////////////////////////////////////////////

mish.c
From main, the program enters an infinite loop controlled by a
continue_loop boolean that the user can manage with the program.

once in the loop, the program will take in user input and pass it
to a parser along with a ParsingResult struct.

The ParsingResult struct stores the results from the parsing and
other data such as number of tokens, the type of command, and an
error message is need be.

The parser separates the input into tokens, storing the struct's
necessary information in the process, afterwhich we return to the
aforementioned while loop.

Once back in the loop, the program does one of 3 things, 
if the type of input was for an internal command then the program
executes the desired command via helper functions.

if the type of input was for an internal command error then the
program will display the error message previously stored in the
ParsingResult struct.

if the type of input was for an external command the program goes
to a helper method, in which the program forks, the child sends the
command to the system then promptly ends. The parent process waits
for the child before continuing.

no known problems exist