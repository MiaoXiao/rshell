#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <queue>
#include "Kirb.h"

using namespace std;

#define MEMORY 50000

//structure for processing segments of a command 
struct task
{
	//stores all arguments
	char* argumentList[MEMORY];
	//size of argumentlist
	unsigned int argumentListSize;
	//connector id
	int connector;
	//vector of all seperators(redirection) in a task
	//if no seperators, vector is empty
	vector<int> seperators;
	//position of said seperator in arglist
	vector<int> seperatorPos;
	//default constructor 
	task():argumentListSize(0), connector(-1), seperatorPos(0){}
};

//opens a file. will create a new file if it does not exist
//specify if you want to append or not
//specify if you should create a new file, if it does not already exist
//returns fd of new file
int openFile(const char* filename, const bool append, const bool create)
{
	int fd = -1;
	if (create && append) fd = open(filename, O_APPEND | O_WRONLY | O_CREAT, 00600);
	else if (create && !append) fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 00600);
	else if (!create && append) fd = open(filename, O_APPEND | O_WRONLY);
	else if (!create && !append) fd = open(filename, O_RDONLY);
	if (fd == -1) perror("Error with open()");
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
	//individual fd
	int w;
	//holds all files to write to
	vector<int> writeto;
	//vector of all pipe seperators
	vector<int> allPipes;
	//short cut for taskList[i].seperatorPos
	int sepPos;
	//will be false if tasks processing should halt
	bool continueTask = true;
	//loop through and process every task
	for (unsigned i = 0; i < taskList.size() && continueTask && taskList[i].argumentList[0] != '\0'; ++i)
	{
		//by default, command will fail
		int status = 1;
		//short cut for whether you should process redirection/piping or not
		bool redirection = !taskList[i].seperators.empty();
		//check for kirb executable
		if (!strcmp(taskList[i].argumentList[0], "kirb")) K.selectCommand(taskList[i].argumentList, status);
		//if either a connector, OR
		//no connector AND no seperators, OR
		//the next next position in arglist is not NULL
		//try to run command
		else if (taskList[i].connector != -1 ||
				(taskList[i].connector == -1 && !redirection) ||
				(i + 1 < taskList[i].argumentListSize))
		{
			//if using seperators, open files
			if (redirection)
			{
				for(unsigned j = 0; j < taskList[i].seperatorPos.size(); ++j)
				{
					sepPos = taskList[i].seperatorPos[j];
				//	cout << "seperator: " << taskList[i].seperators[j] << endl;
					switch(taskList[i].seperators[j])
					{
						case 7:
						case 8:
						case 9:
						case 3: // >
							//cout << "opening: " << taskList[i].argumentList[i + sepPos] << endl;
							w = openFile(taskList[i].argumentList[sepPos], false, true);
							break;
						case 4: // <
							//cout << "opening: " << taskList[i].argumentList[i + sepPos] << endl;
							w = openFile(taskList[i].argumentList[sepPos], false, false);  
							if (w == -1) redirection = false;
							break;
						case 10:
						case 11:
						case 12:
						case 5: // >>
							//cout << "opening: " << taskList[i].argumentList[i + sepPos] << endl;
							w = openFile(taskList[i].argumentList[sepPos], true, true);
							break;
						case 6: // |
							break;
					}
					writeto.push_back(w);
				}
			}
			
			/*//set up files for piping
			int rw[2];
			if (pipe(rw) == -1)
			{
				perror("There was an error with pipe()");
				exit(1);
			}
			writeto.push_back(rw[0]);
			writeto.push_back(rw[1]);
			*/

			//start forking process
			int pid = fork();
			if (pid <= -1) //error
			{
				perror("There was an error with fork()");
				exit(1);
			}
			else if (pid == 0) //child
			{
				int spot;
				//write to correct file if using redirection
				if (redirection)
				{
					for (unsigned j = 0; j < taskList[i].seperatorPos.size(); ++j)
					{
						if (taskList[i].seperators[j] == 7 || taskList[i].seperators[j] == 10) spot = 0;
						else if (taskList[i].seperators[j] == 9 || taskList[i].seperators[j] == 12) spot = 2;
						else spot = 1;

						sepPos = taskList[i].seperatorPos[j];
						//cout << "seppos: " << sepPos << endl;
						switch(taskList[i].seperators[j])
						{
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							case 3: // > 
							case 5: // >>
								closeCheck(spot);
								dupCheck(writeto[j], spot);
								taskList[i].argumentList[sepPos] = '\0';
								break;
							case 4: // <
								closeCheck(0);
								dupCheck(writeto[j], 0);
								taskList[i].argumentList[sepPos + 1] = '\0';
								break;
							case 6: // |
							/*
								closeCheck(1);
								closeCheck(rw[1]);
								dupCheck(rw[0], 1);
								taskList[i].argumentList[sepPos] = '\0';
								*/
								break;
						}
					}
				}

				//try to run the executable/arguments
				if (execvp(taskList[i].argumentList[0], taskList[i].argumentList) == -1)
					perror("There was an error with the executable or argument list");

				exit(1);
			}
			else if (pid > 0) //parent
			{
				if (waitpid(pid, &status, 0) == -1)	perror("Error with waitpid");
				
				/*
				for (unsigned j = 0; j < taskList[i].seperatorPos.size(); ++j)
				{
					//if piping, fork a second time
					if (taskList[i].seperators[j] == 6)
					{
						sepPos = taskList[i].seperatorPos[j];
						pid = fork();
						if (pid <= -1) //error
						{
							perror("There was an error with fork()");
							exit(1);
						}
						else if (pid == 0) //second child
						{
							closeCheck(0);
							closeCheck(rw[0]);
							dupCheck(rw[1], 0);
						
							//test, fill new arg list
							char* newarglist[MEMORY];
							newarglist[0] = taskList[i].argumentList[sepPos];
							newarglist[1] = '\0';

							//try to run the executable/arguments
							if (execvp(taskList[i].argumentList[sepPos], newarglist) == -1)
								perror("There was an error with the executable or argument list");
							
							exit(1);
						}
						else //second parent
						{
							if (waitpid(pid, &status, 0) == -1)	perror("Error with waitpid");
						}
					}
				
				}
*/	
				//close all used fds, including any pipes
				if (redirection)
					for(unsigned j = 0; j < writeto.size(); ++j) closeCheck(writeto[j]);
			}
			writeto.clear();
			//allPipes.clear();
		}
		else
		{
			cout << "Incorrect use of redirection or piping" << endl;
			continueTask = false;
		}
    	//cout << "Status: " << status << endl;
    	if ((status == 0 && taskList[i].connector == 1) || (status > 0 && taskList[i].connector == 2))
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
        else if ((command[i] == '0' || command[i] == '1' || command[i] == '2') &&
				command[i + 1] == '>')
        {
            if (command[i] == '0') fixedCommand[j] = '0';
            else if (command[i] == '1') fixedCommand[j] = '1';
            else if (command[i] == '2') fixedCommand[j] = '2';
            fixedCommand[++j] = '>';
            if (command[i + 2] == '>')
            {
				++i;
				fixedCommand[++j] = '>';
            }
            ++i;
        }
        else fixedCommand[j] = command[i];

        if (command[i + 1] == '\0') fixedCommand[j + 1] = '\0';
    }
    strcpy(command, fixedCommand);
    free(fixedCommand);
}

//check if a snip is a connector
//returns -1 if none 
//returns 0, 1, 2 if it is a ; || or &&
int checkConnector(const char* snip)
{
    if (!strcmp(snip, ";")) return 0;
    else if (!strcmp(snip, "||")) return 1;
    else if (!strcmp(snip, "&&")) return 2;
    return -1;
}

//checks if a snip is a seperator
//return -1 if none
//returns 3 if >
//returns 4 if <
//returns 5 if >>
//returns 6 if |
int checkSeperator(const char* snip)
{
    if (!strcmp(snip, ">")) return 3;
    else if (!strcmp(snip, "<")) return 4;
    else if (!strcmp(snip, ">>")) return 5;
    else if (!strcmp(snip, "||")) return 6;
    else if (!strcmp(snip, "0>")) return 7;
    else if (!strcmp(snip, "1>")) return 8;
    else if (!strcmp(snip, "2>")) return 9;
    else if (!strcmp(snip, "0>>")) return 10;
    else if (!strcmp(snip, "1>>")) return 11;
    else if (!strcmp(snip, "2>>")) return 12;

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
    //set of characters seperated by spaces in a command
    char* snip;
    //id for connector (; || &&)
    int connectorid;
    //id for seperator (< > >> |)
    int seperatorid;
    //current pos of seperator
    int seperatorPosid;
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

		//check if nothing is entered
		if (!strcmp(command, "\0")) finishTask = false; 

        //partition command, add any necessary spaces to separate statements
        fixCommand(command);
		
		//check if nothing is entered
		//if (!strcmp(command, "\0")) finishTask = false; 

	    //Tokenize command
	    snip = strtok(command, "\t ");
	
		//iterate through the entire command
		while (finishTask)
		{
	        //create a new task
			task t;
			processTask = true;
			//reset position of argumentlist
			argpos = 0;
			//reset sepPos
			seperatorPosid = 0;

	        //Iterate through a snip 
            while(snip != NULL && processTask)
	        {
                //get ids for connector and seperator
                connectorid = checkConnector(snip);
                seperatorid = checkSeperator(snip);

				//process executable/arguments
                if (connectorid == -1 && seperatorid == -1) 
                {
                    //possibly run any special commands
					if (!strcmp(snip, "exit")) 
					{
						free(host);
						exit(1);
					}
                    t.argumentList[argpos] = snip;
                    t.argumentListSize++;
                    argpos++;
                    seperatorPosid++;
                }
                else if (connectorid != -1) //process connector
                {
                    t.argumentList[argpos] = '\0';
					t.connector = connectorid;
					
					//finish processing this task, will get a new task
					processTask = false;
                }
                else if (seperatorid != -1) //process seperator
                {
					t.seperators.push_back(seperatorid);
					t.seperatorPos.push_back(seperatorPosid);
                }
				
			    //move to next snippet
			    snip = strtok(NULL, " ");
				
				//if there is a connector, push finished task to vector
				if (connectorid != -1 || snip == NULL)
				{
                    t.argumentList[argpos] = '\0';
					taskList.push_back(t);
				}

                //if last task, run all tasks
                if (snip == NULL)
                {
                	//displayCharArray(t.argumentList);
                	//cout << "size of seperators: " << t.seperators.size() << endl;
                	//cout << "size of arglist: " << t.argumentListSize << endl;
                    if (!t.argumentListSize == 0)runCommand(taskList, K);
                    else cout << "No executable specified" << endl;
                    //all tasks finished, get new tasks from command prompt
                    finishTask = false;
                    processTask = false;
                }
	        }
	    }
	} while (true);
    return 0;
}


