#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <iostream>


using namespace std;

//displays files
//checks flags vector for anything extra
void displayls(dirent* filename, const vector<bool> flags)
{
	//if -a flag is not set, do not ls that file
	if (filename->d_name[0] != '.' || flags[0])
	{
		//check for potential -l flag
		if (flags[1])
		{

		}
		//display name
		cout << filename->d_name << endl; 	

	}
}

//checks for possible path
//returns false if the first character was a -
bool checkPath(char* command)
{
	char* pch;
	pch = strtok(command, " ");
	return (pch[0] != '-');
}

//parses commands to find flags, fills flags vector
//returns false if invalid command
bool parseCommand(char* command, vector<bool> &flags)
{
	//snippet
	char* pch;

	pch = strtok(command, " ");

	while (pch != NULL)
	{
		//make sure flag starts with -
		if (pch[0] == '-')
		{
			if (pch[1] == '\0')
				return false;

			for (int i = 1; pch[i] != '\0'; i++)
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

	//default directory name
	char dirName[] = ".";
	
	if (argc < 1) //error
	{
		cout << "Error opening file" << endl;
	}
	else if (argc > 1) //run ls with path or flags
	{
		int i = 1;
		//initalize potential path
		if (checkPath(argv[i]))
		{
			strcpy(dirName, argv[i]);	
			i++;
		}

		for (; i < argc; i++)
		{
			if (!parseCommand(argv[i], flags))
			{
				cout << "Invalid flag arguments" << endl;
				exit(1);
			}
		}
		//displayFlags(flags);
	}
	//if argc is 1, run normal ls

	//error check opendir
	if (opendir(dirName) == NULL)
	{
		perror("Error with opendir()");
		exit(1);
	}
	//open directory and assign pointer
	DIR *dirp = opendir(dirName);	
	dirent *direntp;

	while ((direntp = readdir(dirp)))
	{
		//error check readdir
		if (direntp < 0)
			perror("Error with readdir()");
	
		//list files
		displayls(direntp, flags);

		//error check closedir
		//if (closedir(dirp) != 0)
		//	perror("Error with closedir()");
	}
}
