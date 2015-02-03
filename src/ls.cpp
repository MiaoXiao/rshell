#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/stat.h>
#include <linux/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;

//given month number, returns month name
string getMonth(unsigned monthnumber)
{
	switch(monthnumber)
	{
		case 0: return "Jan";
		case 1: return "Feb";
		case 2: return "March";
		case 3: return "April";
		case 4: return "May";
		case 5: return "June";
		case 6: return "July";
		case 7: return "Aug";
		case 8: return "Oct";
		case 9:	return "Sep";
		case 10: return "Nov";
		case 11: return "Dec";
	}
	return "ERROR";
}

//returns string of file permissions
string filePermission(mode_t m)
{
	string permissions = "---------- 1";
	if (m & S_IRUSR) permissions[1] = 'r';
	if (m & S_IWUSR) permissions[2] = 'w';
	if (m & S_IXUSR) permissions[3] = 'x';
	if (m & S_IRGRP) permissions[4] = 'r';
	if (m & S_IWGRP) permissions[5] = 'w';
	if (m & S_IXGRP) permissions[6] = 'x';
	if (m & S_IROTH) permissions[7] = 'r';
	if (m & S_IWOTH) permissions[8] = 'w';
	if (m & S_IXOTH) permissions[9] = 'x';
	
	if (S_ISDIR(m))
	{
		permissions[0] = 'd';
		permissions[11] = '2'; 
	}

	return permissions;
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

			//file permissions and inode
			cout << filePermission(info.st_mode) << "\t";

			//user id
			struct passwd userinfo = *getpwuid(info.st_uid);
			cout << userinfo.pw_name << "\t";

			//group id
			struct group groupinfo = *getgrgid(info.st_gid);
			cout << groupinfo.gr_name << "\t";

			//size in bytes
			cout << info.st_size << "\t";

			//get time info
			struct tm lastmodified;
			lastmodified = *gmtime(&info.st_mtime);
			//date of last modified
			cout << getMonth(lastmodified.tm_mon) << " " << lastmodified.tm_mday <<  "\t";
			//time of last modified
			cout << lastmodified.tm_hour << ":" << lastmodified.tm_min << "\t";
		}
		//display name
		cout << filename->d_name << endl; 	
	}
}

//parses commands to find flags, fills flags vector
//fills a container with paths
//returns whether flags are invalid
bool parseCommand(char* command, vector<bool> &flags, vector<string> &paths)
{
	//if command is a flag
	if (command[0] == '-')
	{
		if (command[1] == '\0') return false;
		
		for (int i = 1; command[i] != '\0'; i++)
		{
			if(command[i] == 'a') flags[0] = true;
			else if (command[i] == 'l') flags[1] = true;
			else if (command[i] == 'R') flags[2] = true;
			else return false;
		}
	}
	else //if command is path
	{
		string s(command);
		paths.push_back(s);
		return true;
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

//DEBUG: displays paths array
void displayPaths(const vector<string> v)
{
	for (unsigned i = 0; i < v.size(); i++) cout << i << ": " << v[i] << endl;
	if (!v.size()) cout << "paths vector empty" << endl;
}

int main(int argc, char *argv[])
{
	//pos 0 is -a flag
	//pos 1 is -l flag
	//pos 2 is -R flag
	vector<bool> flags(false);
	flags.resize(3);
	
	//container to hold paths
	vector<string> paths;

	if (argc < 1) //error
	{
		cout << "Error opening file" << endl;
		exit(1);
	}
	else if (argc > 1) //run ls with path or flags
	{
		for (int i = 1; i < argc; i++)
		{
			//check command to see if path, flag, or error.
			//fills corresponding container
			//also updates size of paths array
			if (!parseCommand(argv[i], flags, paths))
			{
				cout << "Invalid flag" << endl;
				exit(1);
			}
		}
		//displayFlags(flags);
	}
    
	//if no paths were given, only use the default path
	if (!paths.size()) paths.push_back(".");
	
	//displayPaths(paths);
    
	//iterate through any directory in the path
	for (unsigned i = 0; i < paths.size(); i++)
	{
		//error check opendir
		if (opendir(paths[i].c_str()) == NULL)
		{
			perror("Error with opendir()");
			exit(1);
		}
		//open directory and assign pointer
		DIR *dirp = opendir(paths[i].c_str());	
		dirent *direntp;
		
		//show directory name if multiple paths
		if (paths.size() > 1) cout << paths[i] << ": " << endl;
		    
		while ((direntp = readdir(dirp)))
		{
			//error check readdir
			if (direntp < 0) perror("Error with readdir()");
			//list files
			displayls(direntp, flags);
		}
		
		if (closedir(dirp) == -1)
		{
			perror("Error with closedir()");
			exit(1);
		}
		
		//new line for next directory (except for last directory)
		if (i + 1 != paths.size()) cout << endl;
	}
}
