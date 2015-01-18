#include <iostream>
#include <unistd.h>
#include <stduo.h>

using namespace std;

int main(char* argc, char** argv)
{
    char* command;
    pch = strtok (command," ");
    //int pid = fork();

    while(command != "exit")
    {
        //Retrieve command
        cout >> "$ ";
        cin >> command;
    }
    if (execvp(cmd, argv)!= 0)
        perror("error in execvp");
}


