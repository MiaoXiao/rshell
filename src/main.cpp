#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

//set to false if shell should exit
bool RUNPROGRAM = true;

void runCommand()
{
   // if (execvp(cmd, argv)!= 0)
     //   perror("error in execvp");

}

//returns true of snippet is a comment
bool checkComment(char* snip)
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
}

void checkConnector(char* snip)
{

}

int main(int argc, char* argv[])
{
	//user input command
    char command[256];
	//snippet of entire command
    char* snip;
	
    do
    {
        //Retrieve command
        cout << "$ ";
        cin.getline(command, 256);

        //Split command
        snip = strtok(command, " ");
        while(snip != NULL)
        {
			cout << snip << endl;
			
			if (snip == "exit")
				RUNPROGRAM = false;
			else if (checkComment(snip))
				cout << "comment" << endl;
			
            snip = strtok(NULL, " ");
        }

    } while (RUNPROGRAM);

    return 0;
}


