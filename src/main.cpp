#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

//set to true to ignore rest of command
bool IGNORE = false;

//run this command given these parameters
//returns whether this operation succeeded or not
bool runCommand(char* executable, char argumentList[], int connector, bool lastOperation)
{
	/*
	pid = fork();
	if (execvp(command, argv)!= 0)
	perror("error in exevp");
	*/
}

//look through command and seperate all statements by spaces
//if there is a # ignore every character after it
//returns the fixed command
void fixCommand(char* command)
{
    char* fixedCommand;
    //j keeps track of position in fixed Command
    //i keeps track of position in command
    for (int i = 0, j = 0; command[i] != '\0'; ++i, ++j)
    {
        if (command[i] == '#')
        {
            command[i] = '\0';
            fixedCommand[j] = '\0';
        }
        else if (command[i] == ';')
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = ';';
            fixedCommand[++j] = ' ';
        }
        else if (command[i] == '|' && command[i + 1] == '|')
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = '|';
            fixedCommand[++j] = '|';
            fixedCommand[++j] = ' ';
            ++i;
        }
        else if (command[i] == '&' && command[i + 1] == '&')
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = '&';
            fixedCommand[++j] = '&';
            fixedCommand[++j] = ' ';
            ++i;
        }
        else
        {
            fixedCommand[j] = command[i];
        }

        if (command[i + 1] == '\0')
            fixedCommand[j + 1] = '\0';
    }
    strcpy(command, fixedCommand);
}

//checks whether the given snip is a connector
//returns -1 if not a connector
//returns 0, 1, 2 if it is a ; || or &&
int checkConnector(char* snip)
{
    if (snip == ";")
        return 0;
    else if (snip == "||")
        return 1;
    else if (snip == "&&")
        return 2;

    return -1;
}

int main(int argc, char* argv[])
{
	//user input command
	char command[256];
	//snippet of entire command
	char* snip;
	//executable
	char* executable = NULL;
	//argument list
	char* argumentList;
	//whether the last operation failed or not
    int connectorFlag = -1;
	bool lastOperation = true;

	do
	{
	    	//keep track of whether you are checking executable, argument, or connector
	    	// 0 is executable, 1 is argument
	        int count = 0;
	        //keep track of size in argument list. resets every command.
	        int argumentListCount = 0;

	        //Retrieve command
	        cout << "$ ";
	        cin.getline(command, 256);

            //fix command, add any neccessary spaces to seperate statements
            fixCommand(command);

	        //Tokenize command
	        snip = strtok(command, " ");

	        //Iterate until entire command is parsed and partitioned
            while(snip != NULL)
	        {
			    //exit shell if input is "exit"
		        if (strcmp(snip, "exit") == 0)
			    	return 0;

			    //if snippet is connector, RUN COMMAND with parameters:

			    //otherwise, init variable with the executable, argument, or connector
			    //1. if executable, simply init executable
			    //2. if argument, add to argument array and increment argument list counter
                connectorFlag = checkConnector(snip);

			    //move to next snippet
			    snip = strtok(NULL, " ");
	        }

	//RUN command if last snippet was NOT a connector

	argumentListCount= 0;
	count = 0;

	} while (1);

    return 0;
}


