#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Kirb.h"

using namespace std;

#define MEMORY 50000

//structure for processing commands
struct task
{
	//stores all arguments
	char* argumentList[MEMORY];
	//seperator id
	int seperator;
};

//opens a file. will create a new file if it does not exist
//specify if you want to append or not
//returns fd of new file
int openFile(const char* filename, const bool append)
{
	int fd;
	if (append) fd = open(filename, O_APPEND | O_WRONLY | O_CREAT, 00600);
	else fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 00600);
	if (fd == -1)
	{
		perror("Error with open()");
		exit(1);
	}
	return fd;
}

//given 2 fds for src and dst, reads from src and writes into dst
void wrFile(const int src, const int dst)
{
	char buf[BUFSIZ];
	int reads;
	int writes;
	while ((reads = read(src, &buf, BUFSIZ)) && (writes = write(dst, &buf, reads)));
}

//dup fds and error check
//returns new fd
int dupCheck(const int o, const int n)
{
	int fd = -1;
	if ((fd == dup2(o, n)) == -1)
	{
		perror("Error with dup2()");
		exit(1);
	}
	return fd;
}

//dup fds and error check
//returns new fd
int dupCheck(const int o)
{
	int fd = -1;
	if ((fd == dup(o)) == -1)
	{
		perror("Error with dup()");
		exit(1);
	}
	return fd;
}

//close file and error check
void closeCheck(const int fd)
{
	if (close(fd) == -1)
	{
		perror("Error with close()");
		exit(1);
	}
}

//run all commands 
void runCommand(vector<task> taskList, Kirb &K)
{
	int writeto;
	//will be false if tasks processing should halt
	bool continueTask = true;
	//loop through and process every task
	for (unsigned i = 0; i < taskList.size() && continueTask; ++i)
	{	
		//by default, command will fail
		int status = 1;
		//check for kirb executable
		if (!strcmp(taskList[i].argumentList[0], "kirb")) K.selectCommand(taskList[i].argumentList, status);
		//if redirection/pipe and not enough arguments, do not run command
		else if (taskList[i].seperator == -1 ||
				taskList[i].seperator == 0 ||
				taskList[i].seperator == 1 ||
				taskList[i].seperator == 2 ||
				i + 1 < taskList.size())
		{
			//check for any piping or redirection
			switch(taskList[i].seperator)
			{	
				case 3: // >
				case 4: // <
					writeto = openFile(taskList[i + 1].argumentList[0] ,false);
					break;
					break;
				case 5: // >>
					writeto = openFile(taskList[i + 1].argumentList[0], true);
					break;
				case 6:
					break;
			}
			
			//start forking process
			int pid = fork();
			if (pid <= -1) //error
			{
				perror("There was an error with fork()");
				exit(1);
			}
			else if (pid == 0) //child
			{
				//write to correct file if using redirection
				switch(taskList[i].seperator)
				{
					case 3:
					case 5: //redirect stdout
						closeCheck(1);
						dupCheck(writeto, 1);
						break;
					case 4: //redirect stdin
						closeCheck(0);
						dupCheck(writeto, 0);
						break;
					case 6:
						break;
				}

				if (execvp(taskList[i].argumentList[0], taskList[i].argumentList) == -1)
					perror("There was an error with the executable or argument list");

				exit(1);
			}
			else if (pid > 0) //parent
			{
				if (waitpid(pid, &status, 0) == -1)	perror("Error with waitpid");
				
				//close fds
				switch(taskList[i].seperator)
				{
					case 3:
					case 5:
						closeCheck(writeto);
						i++;
						break;
					case 4:
						break;
					case 6:
						break;
				}

			}
		}
		else
		{
			cout << "Error: need more arguments for redirection/piping" << endl;
			continueTask = false;
		}
    	//cout << "Status: " << status << endl;
    	if ((status == 0 && taskList[i].seperator == 1) || (status > 0 && taskList[i].seperator == 2))
    		continueTask = false;
    }
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
        if (command[i] == '#') //command recognition
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
//returns -1 if none 
//returns 0, 1, 2 if it is a ; || or &&
//returns 3 if >
//returns 4 if <
//returns 5 if >>
//returns 6 if |
int checkSeperator(const char* snip)
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
void displayCharArray(const char* a[])
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
    //id for seperator (; || && > < etc)
    int seperatorid;
    //whether there are still tasks that must be processed
    bool finishTask;
    //whether you are still processing current task
    bool processTask;
    //arg list position
    int argpos = 0;

	//holds all tasks. cleared when getting new input
	vector<task> taskList;

	do
	{
		taskList.clear();
		finishTask = true;
        argpos = 0;

	    //Retrieve command
	    cout << user << "@" << host << " " << K.displayExpression() << " ";
	    cin.getline(command, MEMORY);

        //partition command, add any necessary spaces to separate statements
        fixCommand(command);
	    //Tokenize command
	    snip = strtok(command, "\t ");

		while (finishTask)
		{
	        //create a new task
			task t;
			processTask = true;

	        //Iterate until entire command is parsed
            while(snip != NULL && processTask)
	        {
                //cout << "Snip:" << snip << endl;
                seperatorid = checkSeperator(snip);

                //cout << connectorFlag << endl;
                if (seperatorid == -1) //fill struct with arguments
                {
                    //possibly run any special commands
					if (!strcmp(snip, "exit")) //terminates shell
					{
						free(host);
						exit(1);
					}
					t.seperator = seperatorid;
                    t.argumentList[argpos] = snip;
                    argpos++;
                }
                else //finish struct and add to taskList
                {
                    //displayCharArray(argumentListc);
                    t.argumentList[argpos] = '\0';
                    argpos = 0;
					t.seperator = seperatorid;
					taskList.push_back(t);
					
					//finish processing this task
					processTask = false;
                }

			    //move to next snippet
			    snip = strtok(NULL, " ");

                //check to see if that was the last task
                if (snip == NULL)
                {
                	//if last task had no connector at the end, push back task
                	if (t.seperator == -1) taskList.push_back(t);
                    runCommand(taskList, K);
                    //all tasks finished, get new tasks from command prompt
                    finishTask = false;
                }
	        }
	    }
	} while (true);
    return 0;
}


