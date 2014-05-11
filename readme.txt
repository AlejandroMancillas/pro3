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

github used as a verion control method

no known problems exist

example interaction:

mish[1]> echo so, does this work okay
so, does this work okay
mish[2]> echo this is a pretty lengthy thing I am typing here for no good reason at all
this is a pretty lengthy thing I am typing here for no good reason at all
mish[3]> verbose on
        command: verbose on

        input command tokens:
        0: verbose
        1: on
mish[4]> echo yup, verbose up and running
        command: echo yup, verbose up and running

        input command tokens:
        0: echo
        1: yup,
        2: verbose
        3: up
        4: and
        5: running
        wait for pid 14931: echo
        execvp: echo
yup, verbose up and running
command status 0
mish[5]> bogus
        command: bogus

        input command tokens:
        0: bogus
        wait for pid 14933: bogus
        execvp: bogus
sh: 1: bogus: not found
command status 32512
mish[6]> date
        command: date

        input command tokens:
        0: date
        wait for pid 14936: date
        execvp: date
Sun May 11 12:43:06 EDT 2014
command status 0
mish[7]> which grep
        command: which grep

        input command tokens:
        0: which
        1: grep
        wait for pid 14940: which
        execvp: which
/bin/grep
command status 0
mish[8]> cat header.mak
        command: cat header.mak

        input command tokens:
        0: cat
        1: header.mak
        wait for pid 14943: cat
        execvp: cat
CC =            gcc
CFLAGS =        -Wall -ggdb -std=c99
command status 0
mish[9]> verbose something
        command: verbose something

        input command tokens:
        0: verbose
        1: something
        usage: verbose [on|off]
mish[9]> history
        command: history

        input command tokens:
        0: history
        Displaying history:
                1: echo so, does this work okay
                2: echo this is a pretty lengthy thing I am typing here for no good reason at all
                3: verbose on
                4: echo yup, verbose up and running
                5: bogus
                6: date
                7: which grep
                8: cat header.mak
                9: verbose something
                10: history
mish[10]> ls missingfile
        command: ls missingfile

        input command tokens:
        0: ls
        1: missingfile
        wait for pid 14946: ls
        execvp: ls
ls: cannot access missingfile: No such file or directory
command status 512
mish[11]> verbose off
mish[12]> help
        Displaying Help:
                verbose:        usage: verbose [on|off]
                        When on, the program displays additional.
                        information.
                help:           usage: help
                        displays information on internal commands.
                history:        usage: history
                        print a list of commands executed so far,.
                        including their arguments.
                quit:           usage: quit
                        terminates the mish shell.
mish[13]> quit
