#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;

//given month number, returns month name
string getMonth(unsigned monthnumber)
{
	switch(monthnumber)
	{
		case 0:
			return "Jan";
		case 1:
			return "Feb";
		case 2:
			return "March";
		case 3:
			return "April";
		case 4:
			return "May";
		case 5:
			return "June";
		case 6:
			return "July";
		case 7:
			return "Aug";
		case 8:
			return "Oct";
		case 9:
			return "Sep";
		case 10:
			return "Nov";
		case 11:
			return "Dec";
	}
	return "Error getting month name";
}

//displays files
//checks flags vector for anything extra
void displayls(dirent* filename, const vector<bool> flags)
{
	//if -a flag is not set, do not ls a file that starts with . 
	if (filename->d_name[0] != '.' || flags[0])
	{
		//check for potential -l flag
		if (flags[1])
		{
			//get -l info on
			struct stat info;

			//perror
			if (stat(filename->d_name, &info) == -1)
			{
				perror("There was a problem with stat()");
				exit(1);
			}

			//get time info
			struct tm lastmodified;
			lastmodified = *gmtime(&info.st_mtime);
			//check for day light savings
			bool dls = false;
			if (lastmodified.tm_isdst > 0)
				dls = true;
			
			//user id
			cout << info.st_uid << "\t";
			//group id
			cout << info.st_gid << "\t";
			//size in bytes
			cout << info.st_size << "\t";
			//date of last modified
			cout << getMonth(lastmodified.tm_mon) << " " << lastmodified.tm_mday <<  "\t";
			//time of last modified
			cout << lastmodified.tm_hour - dls << ":" << lastmodified.tm_min << "\t";

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
