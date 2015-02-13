#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Kirb.h"

using namespace std;

#define MEMORY 50000

//run this command given these parameters
//returns whether this operation succeeded or not based off the connector
bool runCommand(char* executable, char* argumentList[], int connector, Kirb &K)
{
	//by default, command will fail
	int status = 1;
	//check for kirb executable
	if (!strcmp(executable, "kirb")) K.selectCommand(argumentList, status);
	else
	{
		//check if you should possibly run a command or not
		//Decide if you should run the next command
		int pid = fork();
		if (pid <= -1) //error
		{
			perror("There was an error with fork()");
			exit(1);
		}
		else if (pid == 0) //child
		{
			if (execvp(executable, argumentList) == -1)
				perror("There was an error with the executable or argument list");
			exit(1);
		}
		else if (pid > 0) //parent
			if (waitpid(pid, &status, 0) == -1)	perror("Error with waitpid");
	}
    //cout << "Status: " << status << endl;
    return (!(status == 0 && connector == 1) || (status > 0 && connector == 2));
}

//look through command and separate all statements by spaces
//if there is a # ignore every character after it
void fixCommand(char* command)
{
    char* fixedCommand =(char*)malloc(MEMORY);
    //j keeps track of position in fixed Command
    //i keeps track of position in command
    for (int i = 0, j = 0; command[i] != '\0'; ++i, ++j)
    {
        if (command[i] == '#') //cpp,,amd recognition
        {
            command[i] = '\0';
            fixedCommand[j] = '\0';
        }
        else if (command[i] == ';')//; connector
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = ';';
            fixedCommand[++j] = ' ';
        }
        else if (command[i] == '|' && command[i + 1] == '|')//|| connector
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = '|';
            fixedCommand[++j] = '|';
            fixedCommand[++j] = ' ';
            ++i;
        }
        else if (command[i] == '|')//piping
        {
			fixedCommand[j] = ' ';
			fixedCommand[++j] = '|';
			fixedCommand[++j] = ' ';
        }
        else if (command[i] == '&' && command[i + 1] == '&')//&& connector
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = '&';
            fixedCommand[++j] = '&';
            fixedCommand[++j] = ' ';
            ++i;
        }
        else if (command[i] == '>' && command[i + 1] == '>')//stdout redirection append
        {
            fixedCommand[j] = ' ';
            fixedCommand[++j] = '>';
            fixedCommand[++j] = '>';
            fixedCommand[++j] = ' ';
            ++i;
        }
        else if (command[i] == '>')//stdout redirection
        {
			fixedCommand[j] = ' ';
			fixedCommand[++j] = '>';
			fixedCommand[++j] = ' ';

        }
        else if (command[i] == '<')//stdin redirection
        {
			fixedCommand[j] = ' ';
			fixedCommand[++j] = '<';
			fixedCommand[++j] = ' ';
        }
        else fixedCommand[j] = command[i];

        if (command[i + 1] == '\0') fixedCommand[j + 1] = '\0';
    }
    strcpy(command, fixedCommand);
    free(fixedCommand);
}

//checks whether the given snip is a connector/redirection
//returns -1 if not a connector
//returns 0, 1, 2 if it is a ; || or &&
//returns 3 if >
//returns 4 if <
//returns 5 if >>
//returns 6 if |
int checkConnector(char* snip)
{
    if (!strcmp(snip, ";")) return 0;
    else if (!strcmp(snip, "||")) return 1;
    else if (!strcmp(snip, "&&")) return 2;
    else if (!strcmp(snip, ">")) return 3;
    else if (!strcmp(snip, "<")) return 4;
    else if (!strcmp(snip, ">>")) return 5;
    else if (!strcmp(snip, "|")) return 6;
    return -1;
}

//DEBUG:displays char array
void displayCharArray(char* a[])
{
    for (int i = 0; a[i] != NULL; i++)
        cout << i << ": " << a[i] << endl;
}

int main(int argc, char* argv[])
{
    //info for login and host
    char* host = (char*)malloc(300);
    string user;

    //error checking
    if (getlogin() != NULL) user = getlogin();
    else perror("Error with getting user");
    if (gethostname(host, 300) == -1) perror("Error with getting host name");
	
	//class for handling kirb's expression
	Kirb K;
	//user input command
	char command[MEMORY];
    //snippet of the command
    char* snip;
	//whether the snip is a connector, redirection, or neither 
    int connectorFlag = -1;
    //whether you should terminate parsing a command early or not
    bool continueParsing = true;
    //vector for all arguments
    char* argumentListc[MEMORY];
    //arg list position
    int argpos = 0;
    //keep track of whether you are checking executable or argument
    // 0 is executable, 1 is argument
    int statement = 0;

	do
	{
            continueParsing = true;
            statement = 0;
            argpos = 0;

	        //Retrieve command
	        cout << user << "@" << host << " " << K.displayExpression() << " ";
	        cin.getline(command, MEMORY);

            //partition command, add any necessary spaces to separate statements
            fixCommand(command);
	        //Tokenize command
	        snip = strtok(command, "\t ");

	        //Iterate until entire command is parsed
            while(snip != NULL && continueParsing)
	        {
                //cout << "Snip:" << snip << endl;
                connectorFlag = checkConnector(snip);

                //check if flag is a pipe or redirection
                //if (connectorFlag != 0 && connectorFlag != 1 && connectorFlag != 2 && connectorFlag != -1)
	
                //cout << connectorFlag << endl;
                if (statement == 0 && connectorFlag == -1)
                {
                    //possibly run any special commands
					if (!strcmp(snip, "exit")) //terminates shell
					{
						free(host);
						exit(1);
					}

                    argumentListc[argpos] = snip;
                    argpos++;
                    statement++;
                    //cout << "Parsed executable" << endl;
                }
                else if (statement == 1 && connectorFlag == -1)
                {
                    argumentListc[argpos] = snip;
                    argpos++;
                    //cout << "Parsed argument" << endl;
                }
                else if (connectorFlag != -1)
                {
                    //displayCharArray(argumentListc);
                    //cout << "Connector: " << connectorFlag << endl;
                    argumentListc[argpos] = '\0';
                    statement = 0;
                    argpos = 0;
                    if(!runCommand(argumentListc[0], argumentListc, connectorFlag, K)) continueParsing = false;
                }

			    //move to next snippet
			    snip = strtok(NULL, " ");
                //check if this is the last command to run
                if (connectorFlag == -1 && snip == NULL)
                {
                    argumentListc[argpos] = '\0';
                    //displayCharArray(argumentListc);
                    runCommand(argumentListc[0], argumentListc, connectorFlag, K);
                }
	        }
	} while (1);
    return 0;
}


