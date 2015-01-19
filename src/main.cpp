#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

void runCommand()
{
   // if (execvp(cmd, argv)!= 0)
     //   perror("error in execvp");

}

int main(int argc, char* argv[])
{
    char* command = "ls -as";
    char* pch;

    do
    {
        //Retrieve command
        cout << "$ ";
        getline(cin, command);

        //tokenize command
        pch = strtok(command," ");
        while(pch != NULL)
        {
            cout << pch << endl;
            pch = strtok(NULL, " ");
        }

    } while (command != "exit");

    return 0;
}


