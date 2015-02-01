#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

//parses commands to find flags, fills flags vector
//returns true/false if invalid command
bool parseCommand(char* command, vector<bool> &flags)
{
	//snippet
	char* pch;

	pch = strtok(command, " ");
	while (pch != NULL)
	{
		//make sure flag starts with -
		if (pch[1] == '-')
		{
			for (int i = 1; pch[i] != ' '; i++)
			{
				if(pch[i] == 'a')
					flags[0] = true;
				else if (pch[i] == 'l')
					flags[1] = true;
				else if (pch[i] == 'R')
					flags[2] = true;
				else
					return false;
			}
			pch = strtok(NULL, " ");
		}
		else
			return false;	
	}
	return true;
}

//DEBUG: displays flag vector
void displayFlags(const vector<bool> flags)
{
	cout << "-a: " << flags[0] << endl;
	cout << "-l: " << flags[1] << endl;
	cout << "-R: " << flags[2] << endl;
}

int main(int argc, char *argv[])
{
	//pos 0 is -a flag
	//pos 1 is -l flag
	//pos 2 is -R flag
	vector<bool> flags(false);
	flags.resize(3);

	//directory name
	char dirName[] = ".";

	//error check opendir
	if (opendir(dirName) == NULL)
		perror("Error with opendir()");

	//open directory and assign pointer
	DIR *dirp = opendir(dirName);	
	dirent *direntp;

	while ((direntp = readdir(dirp)))
	{
		//error check readdir
		if (direntp < 0)
			perror("Error with readdir()");

		cout << direntp->d_name << endl; // use stat here to find attributes of file
		
		//error check closedir
		if (closedir(dirp) != 0)
			perror("Error with closedir()");
	}
}
