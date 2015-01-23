#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

//run this command given these parameters
//returns whether this operation succeeded or not based off the connector
bool runCommand(char* executable, char* argumentList[], int connector)
{
    int status;
    bool operationSuccess;
    //check if you should possibly run a command or not
    //Potentially run command only if there is no connector, the connector is a ;

    int pid = fork();

    if (pid <= -1) //error
    {
        perror("There was an error with fork().");
        exit(1);
    }
    else if (pid == 0) //child
    {
        if (execvp(executable, argumentList) == -1)
            perror("There was an error with the executable or argument list.");

        exit(1);
    }
    else if (pid > 0) //parent
    {
        int result = waitpid(pid, &status, 0);

        if (result == -1)
            perror("Error with waitpid");
    }

    //cout << "Status: " << status << endl;
    if ((status == 0 && connector == 1) ||
        (status > 0 && connector == 2))
    {
        operationSuccess = false;
        //cout << "Operation failed" << endl;
    }
    else
    {
        operationSuccess = true;
        //cout << "Operation success" << endl;
    }

    return operationSuccess;
}


//look through command and seperate all statements by spaces
//if there is a # ignore every character after it
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
    if (!strcmp(snip, ";"))
        return 0;
    else if (!strcmp(snip, "||"))
        return 1;
    else if (!strcmp(snip, "&&"))
        return 2;

    return -1;
}

//displays char array
void displayCharArray(char* a[])
{
    for (int i = 0; a[i] != NULL; i++)
    {
        cout << i << ": " << a[i] << endl;
    }
}
/*
//changes vector of char* to a char* array
char** finishArgList(vector<char*> v)
{
    cout << "Argument List" << endl;
    char** argumentListc = new char*[v.size() + 1];
    for (int i = 0, j = 0; i < v.size(); i++, j++)
    {
        argumentListc[i] = v[i].snipc;
        cout << i << ": " << argumentListc[i] << endl;
    }

    argumentListc[v.size()] = '\0';
    return argumentListc;
}
*/


int main(int argc, char* argv[])
{
	//user input command
	char command[50000];
    //snippet of the command
    char* snip;
	//whether the last operation failed or not
    int connectorFlag = -1;
    //whether the last operation succeeeded(true) or not(false)
	bool lastOperation = true;
    //whether you should terminate parsing a command early or not
    bool continueParsing = true;
    //vector for all arguments
    char* argumentListc[50000];
    //arg list position
    int argpos = 0;
    //keep track of whether you are checking executable or argument
    // 0 is executable, 1 is argument
    int statement = 0;

	do
	{
            lastOperation = true;
            continueParsing = true;

	        //Retrieve command
	        cout << "$ ";
	        cin.getline(command, 50000);

            //partition command, add any neccessary spaces to seperate statements
            fixCommand(command);

	        //Tokenize command
	        snip = strtok(command, " ");

	        //Iterate until entire command is parsed
            while(snip != NULL && continueParsing)
	        {
			    //init variable with the executable, argument, or connector
			    //1. if executable, simply init executable
			    //2. if argument, add to argument array and increment argument list statementer
                //3. connector, run the current command with runCommand
                //cout << "Snip:" << snip << endl;
                connectorFlag = checkConnector(snip);
                //cout << connectorFlag << endl;
                if (statement == 0 && connectorFlag == -1)
                {
                    //exit shell if input is "exit"
		            if (strcmp(snip, "exit") == 0)
			    	    return 0;

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
                    //create char array that matches the argument list vector
                    //save whether that operation succeeded or failed
                    argumentListc[argpos] = '\0';
                    //displayCharArray(argumentListc);
                    //cout << "Connector: " << connectorFlag << endl;
                    lastOperation = runCommand(argumentListc[0], argumentListc, connectorFlag);
                    statement = 0;
                    argpos = 0;
                    if (!lastOperation)
                        continueParsing = false;

                }
                else
                {
                    cout << "ERROR! Did not parseastatement" << endl;
                }
			    //move to next snippet
			    snip = strtok(NULL, " ");
                if (connectorFlag == -1 && snip == NULL)
                {
                    argumentListc[argpos] = '\0';
                    //displayCharArray(argumentListc);
                    lastOperation = runCommand(argumentListc[0], argumentListc, connectorFlag);
                    statement = 0;
                    argpos = 0;
                }

	        }

	} while (1);

    return 0;
}


