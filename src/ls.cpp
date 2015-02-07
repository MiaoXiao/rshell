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
#include <algorithm>
#include <ctype.h>
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
	return "Error with getting month name";
}

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

/*
//fill paths vector with new paths based on -R flag
void findallpaths(vector<string> &paths, string &pathsofar)
{
	//check if you must check only the . dir, or every dir
	vector<string> recursepaths;
	for (unsigned i = 0; i < paths.size(); ++i)
	{
		if (paths[i] == ".")
		{
			recursepaths.push_back(paths[i]);
			i = paths.size();
		}
	}

	//if no . directory found, recurse on all paths given
	//if (!recursepaths.size()) recursepaths = paths;

	//begin reursing through all possible directories
	for (unsigned i = 0; i < paths.size(); ++i)
	{
		pathsofar = paths[i];

		DIR *dirp = opendir(paths[i].c_str());
		dirent *direntp;
		struct stat info;
		while((direntp = readdir(dirp)))
		{
			stat(direntp->d_name, &info);
			if (S_ISDIR(info.st_mode))
			{
				pathsofar += "/";
				pathsofar.append(direntp->d_name);
				findallpaths(paths, pathsofar);
			}
		}
		closedir(dirp);	
	}
	
}
*/

//returns number of hard links
int findhardlinks(string dir)
{
	struct stat inodeinfo;
	int numbhardlinks = 2;

	//error check opendir
	DIR *tdirp = opendir(dir.c_str());
	if (tdirp == NULL)
	{
		perror("Error with opendir()");
		exit(1);
	}

	dirent *tdirentp;
	//cout << "checking dir: " << dir << endl;
	while ((tdirentp = readdir(tdirp)))
	{
		if (tdirentp < 0)
		{
			perror("Error with readdir()");
			exit(1);
		}

		//cout << "file in dir: " << tdirentp->d_name << endl;

		//insert path at start of string
		string fileindir(tdirentp->d_name);

		//make sure path included is not . or ..
		if (fileindir != "." && fileindir != "..")
		{
			fileindir.insert(0, dir + "/");
			//error check stat
			if (stat(fileindir.c_str(), &inodeinfo) == -1)
			{
				perror("Error with stat()");
				exit(1);
			}
			//if current file is a directory, increase number of hard links
			if (S_ISDIR(inodeinfo.st_mode))
			{
				++numbhardlinks;
			//	cout << endl <<  "file added: " << tdirentp->d_name << endl;
			}
		}
	}
	//error check closed dir
	if (closedir(tdirp) == -1)
	{
		perror("Error with closedir()");
		exit(1);
	}

	return numbhardlinks;
}

//displays all files in a vector files
//checks flags vector for anything extra
void displayls(vector<string> filenames, const vector<bool> flags, string currentpath)
{
	//struct for storing stat info
	struct stat info;
	for (unsigned i = 0; i < filenames.size(); ++i)
	{
		//cout << "Filename: " << filenames[i] << endl;	
		//if -a flag is not set, do not ls a file that starts with . 
		if (filenames[i].at(0) != '.' || flags[0])
		{
			//check for potential -l flag
			if (flags[1])
			{
				//insert current path at start of string
				filenames[i].insert(0, currentpath + "/");
				//cout << "checking file: " << filenames[i] << endl;

				//error check stat
				if (stat(filenames[i].c_str(), &info) != 0)
				{
					perror("Error with stat()");
					exit(1);
				}

				//file permissionse
				cout << filePermission(info.st_mode) << "\t";

				//inode number
				(!S_ISDIR(info.st_mode)) ? cout << 1 : cout << findhardlinks(filenames[i]);
				cout << "\t";

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
			cout << filenames[i] << endl; 	
		}
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

//	string pathsofar = "";
	//if -R flag passed in, find all potential paths
//	if (flags[2]) findallpaths(paths, pathsofar);

	//iterate through every path 
	for (unsigned i = 0; i < paths.size(); ++i)
	{
		//show directory name if multiple paths
		if (paths.size() > 1) cout << paths[i] << ": " << endl;

		//open directory and assign pointer
		DIR *dirp = opendir(paths[i].c_str());
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
		displayls(filenames, flags, paths[i]);

		//close directory
		if (closedir(dirp) == -1)
		{
			perror("Error with closedir()");
			exit(1);
		}
		
		filenames.clear();

		//new line for next directory (except for last directory)
		if (i + 1 != paths.size()) cout << endl;
	}
}
