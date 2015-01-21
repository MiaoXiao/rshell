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
bool runCommand(char* executable, char[] argumentList, int connector, bool lastOperation)
{
	/*
	pid = fork();
	if (execvp(command, argv)!= 0)
	perror("error in exevp");
	*/
}

//Removes any # from a snip, and also any character after said #. Ignore rest of command.
//Also removes any ; || or && appended at the end of snip
//returns 0 if ;, 1 if ||, 2 if &&
//returns -1 if comment
//returns 3 if no fix needed
int fixSnip(char* &snip)
{
	//do something if these special characters are found
	char key[] = "#";
	char* schar;

	schar = strpbrk (snip, key);
	if (schar != NULL)
	{
		IGNORE = true;
		return -1;
	}
	else if (snip[strlen(snip) - 1] == ';')
		return 0;
	else if (snip[strlen(snip) - 1] == '|' && snip[strlen(snip) - 2] == '|')
		return 1;
	else if (snip[strlen(snip) - 1] == '&' && snip[strlen(snip) - 2] == '&')
		return 2;
	
	return 3;
}

int main(int argc, char* argv[])
{
	//user input command
	char command[256];
	//snippet of entire command
	char* snip;
	//executable
	char* executable;
	//argument list
	char* argumentList[256];
	//whether the last operation failed or not
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
	
	        //Tokenize command
	        snip = strtok(command, " ");
	        
	        //Iterate until entire command is parsed and partitioned
		while(snip != NULL)
	        {
	        	//record keeps track of whether there was a comment, connector, or there was no fix in the last snippet
			int record;
			record = fixSnip(snip);
			
			//exit shell if input is "exit"
		        if (strcmp(snip, "exit") == 0)
				return 0;
			
			//if snippet is connector, RUN COMMAND with parameters:
			//executable, argument list array, connector, whether the last argument failed or not
			//running the command will return whether the operation failed or not
			
			//otherwise, init variable with the executable or argument.
			//1. if executable, simply init executable
			//2. if argument, add to argument array and increment argument list counter
			
			//if there was a connector
			
			//move to next snippet
			snip = strtok(NULL, " ");
	        }
	//RUN command if last snippet was NOT a connector
	
	argumentListCount= 0;
	count = 0;
	
	} while (1);

    return 0;
}


