#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


void runCommand(char command)
{
    /*
    pid = fork();
    if (execvp(command, argv)!= 0)
        perror("error in exevp");
*/
}

//Fixes snip and removes any # and characters after the #
//Also removes the any ;, ||, or && at the end of the snip
void fixSnip(char* &snip)
{
	//do something if these special characters are found
	char key[] = "#";
	char* schar;

	schar = strpbrk (snip, key);
	if (schar != NULL)
	{
		return true;
	}
	return false;
    if (snip[strlen(snip) - 1] == ';' ||
        (snip[strlen(snip) - 1] == '|' &&
        snip[strlen(snip) - 2] == '|') ||
        (snip[strlen(snip) - 1] == '&' &&
        snip[strlen(snip) - 2] == '&'))
    {
    }
    else
    {
    }
}

void checkConnector(char* snip)
{

}

void checkArgumentList()
{

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

    do
    {
        int count = 0;
        int argumentListCount = 0;

        //Retrieve command
        cout << "$ ";
        cin.getline(command, 256);

        //Split command
        snip = strtok(command, " ");
        //Iterate until entire command is parsed and partition command
        while(snip != NULL)
        {
			cout << snip << endl;

            //check snip:
			if (strcmp(snip, "exit") == 0)
                return 0;
			else if (fixSnip(snip))
            {
				cout << "comment" << endl;
            }

            //partition commands
            while(count <= 1)
            {
                switch(count)
                {
                    case 0:
                        executable = snip;
                        count++;
                        break;
                     case 1:

                }
            }
            snip = strtok(NULL, " ");
        }

    } while (1);

    return 0;
}


