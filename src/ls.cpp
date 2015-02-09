#include <sys/types.h>
#include <stack>
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
#include <algorithm>
#include <ctype.h>
#include <vector>
#include <iostream>

using namespace std;

//returns string of file permissions
string filePermission(mode_t m)
{
	string permissions = "----------";
	if (S_ISDIR(m)) permissions[0] = 'd';
	else if (S_ISBLK(m)) permissions[0] = 'b';
	else if (S_ISCHR(m)) permissions[0] = 'c';
	else if (S_ISLNK(m)) permissions[0] = 'l';
	else if (S_ISFIFO(m)) permissions[0] = 'p';
	else if (S_ISSOCK(m)) permissions[0] = 's';

	if (m & S_IRUSR) permissions[1] = 'r';
	if (m & S_IWUSR) permissions[2] = 'w';
	if (m & S_IXUSR) permissions[3] = 'x';
	if (m & S_IRGRP) permissions[4] = 'r';
	if (m & S_IWGRP) permissions[5] = 'w';
	if (m & S_IXGRP) permissions[6] = 'x';
	if (m & S_IROTH) permissions[7] = 'r';
	if (m & S_IWOTH) permissions[8] = 'w';
	if (m & S_IXOTH) permissions[9] = 'x';
	
	return permissions;
}

//displays certain files in specific colors
string displayColorText(mode_t m, string filename)
{
	//cout << "filename: " << filename << endl;
	string color = "39";
	string bg = "49";
	//color
	if (S_ISDIR(m)) color = "34"; 
	else if (m & S_IXUSR) color =  "32";
	else if (S_ISBLK(m)) color = "33";
	else if (S_ISCHR(m)) color = "35";
	else if (S_ISLNK(m)) color = "31";
	else if (S_ISFIFO(m)) color = "36";
	else if (S_ISSOCK(m)) color = "92";
	//background
	if (filename.at(0) == '.') bg = "100";
		
	return "\033[1;" + bg + ";" + color + "m" + filename + "\033[0;00m";
}

//displays all files in a vector files
//checks flags vector for anything extra
void displayls(vector<string> filenames, const vector<bool> flags, string currentpath, stack<string> &rdir)
{
	//struct for storing stat info
	struct stat info;
	//total number of blocks
	int totalblocks = 0;
	for (unsigned i = 0; i < filenames.size(); ++i)
	{
		//cout << "Filename: " << filenames[i] << endl;	
		//if -a flag is not set, do not ls a file that starts with . 
		if (filenames[i].at(0) != '.' || flags[0])
		{
			//insert current path at start of string
			string updatedpath(currentpath + "/" + filenames[i]);

			//filenames[i].insert(0, currentpath + "/");
			//cout << "checking file: " << filenames[i] << endl;

			//error check stat
			if (stat(updatedpath.c_str(), &info) != 0)
			{
				perror("Error with stat()");
				exit(1);
			}

			//if -R flag and directory, and the filename is NOT . or ..,
			//push into rdir stack
			if (flags[2] && S_ISDIR(info.st_mode) && 
				filenames[i] != "." && filenames[i] != "..") 
			{
				rdir.push(updatedpath);
				//cout << "Filenames: " << filenames[i] << endl;
			}

			//check for potential -l flag
			if (flags[1])
			{
				//file permissionse
				cout << filePermission(info.st_mode) << "\t";

				//number of hard links
				cout << info.st_nlink << "\t";

				//user id and error check
				struct passwd userinfo = *getpwuid(info.st_uid);
				if (&userinfo == NULL)
				{
					perror("Error with getpwuid()");
					exit(1);
				}
				cout << userinfo.pw_name << "\t";

				//group id and error check
				struct group groupinfo = *getgrgid(info.st_gid);
				if (&groupinfo == NULL)
				{
					perror("Error with getgrgid()");
					exit(1);
				}
				cout << groupinfo.gr_name << "\t";

				//size in bytes
				cout << info.st_size << "\t";

				//get time info and error check
				char* timeinfo = ctime(&info.st_mtime);
				if (timeinfo == NULL)
				{
					perror("Error with ctime()");
					exit(1);
				}

				string displaytime(timeinfo);
				//only display, date and time modified
				cout << displaytime.substr(4, 12) << "\t";

				//get blocks for this file
				totalblocks += info.st_blocks;
			}
			//display name
			cout << displayColorText(info.st_mode, filenames[i]) << endl;
		}
	}
	//display total blocks only if -l flag is passed
	if (flags[1]) cout << "Total blocks: " << totalblocks / 2 << endl;
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

//DEBUG: displays vector of strings 
void displayVector(const vector<string> v)
{
	for (unsigned i = 0; i < v.size(); ++i) cout << i << ": " << v[i] << endl;
	if (!v.size()) cout << "Vector empty" << endl;
}

//comparator for sort
bool scompare(string a, string b)
{
	for (unsigned i = 0; i < a.size(); ++i) a[i] = tolower(a[i]);
	for (unsigned i = 0; i < b.size(); ++i) b[i] = tolower(b[i]);
	return a < b;
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

	//container to hold filenames for each directory
	vector<string> filenames;

	if (argc < 1) //error
	{
		cout << "Error opening file" << endl;
		exit(1);
	}
	else if (argc > 1) //run ls with path or flags
	{
		for (int i = 1; i < argc; ++i)
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
	}
	//if no paths were given, use default path  	
	if (!paths.size()) paths.push_back("."); 	

	//displayVector(paths);
	//displayFlags(flags);	

	//create pointer to directory to be read
	dirent *direntp;
	
	//holds all dirs about to be processed
	stack<string> rdir;
	//current dir to be processed
	string processdir;

	//push all paths from vector to a stack
	for (int i = paths.size() - 1; i >= 0; --i) rdir.push(paths[i]);

	//while the stack of paths is not empty, ls files 
	while (!rdir.empty())
	{
		//reassign dir about to be processed
		processdir = rdir.top();
		//remove directory that is about to be processed from stack
		rdir.pop();
		
		//display directory about to ls
		cout << processdir << ": " << endl;

		//open directory and assign pointer
		DIR *dirp = opendir(processdir.c_str());
		if (dirp == NULL)
		{
			perror ("Error with opendir()");
			exit(1);
		}
	
		//fill filenames vector with all file names
		while ((direntp = readdir(dirp)))
		{
			//error check readdir
			if (direntp < 0)
			{
				perror("Error with readdir()");
				exit(1);
			}
			string s(direntp->d_name);
			filenames.push_back(s);
		}
		
		//alphabetize filenames
		sort(filenames.begin(), filenames.end(), scompare);
		//list files, based on flags, for the given path
		//if -R flag was passed, rdir may fill with additional paths
		displayls(filenames, flags, processdir, rdir);
		
		//close directory
		if (closedir(dirp) == -1)
		{
			perror("Error with closedir()");
			exit(1);
		}
		
		//clear processed filenames
		filenames.clear();

		//newline for every dir except last dir
		if (!rdir.empty()) cout << endl;
	}
}
