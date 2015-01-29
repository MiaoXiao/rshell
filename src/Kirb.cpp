#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "Kirb.h"

using namespace std;

void Kirb::selectCommand(char* argumentList[], int &status)
{
	kirbstatus = status;
	for (int i = 1; argumentList[i] != '\0'; i++)
	{
		string str(argumentList[i]);
		sv.push_back(str);
	}

	if (sv.size() >= 2)
	{
		//cout << "sv1: " << sv[0] << endl;
		if (sv[0] == "feed")
			feedFood(sv[1]);
		else if (sv[0] == "changename")
			changeName(sv[1]);
		else
		{
			cout << "Invalid kirb command: that kirb function does not exist" << endl;
			status = 1;
		}
	}
	else
	{
		cout << "Invalid kirb command: not enough arguments" << endl;
		status = 1;
	}
	
	sv.clear();
	status = kirbstatus;
}

void Kirb::feedFood(string food)
{
	if (food == "peppers")
		changeExpression(2);
	else if (food == "candy")
		changeExpression(3);
	else if (food == "waddledee")
		changeExpression(2);
	else if (food == "fire")
		changeExpression(4);
	else if (food == "water")
		changeExpression(0);
	else
		changeExpression(5);
}

void Kirb::changeName(string newname)
{
	if (!newname.size())
		cout << "Type \"kirb changename NAME\" to change " << name << "'s name." << endl;
	else if (newname.size() > 20)
		cout << name << " doesn't like long names! [New name should be 20 characters or less]" << endl;
	else
	{
		cout << name << "'s name is now ";
		name = newname; 
		cout << newname << "!" << endl;
	}

	sv.clear();
}

void Kirb::changeExpression(unsigned int face)
{
	switch(face)
	{
		//default
		case 0:
			expression = "(>'.')>";
			break;
		//sucking
		case 1:
			expression = "(>'o')>";
			break;
		//munching
		case 2:
			expression = "(>'~')>";
			break;
		//happy
		case 3:
			expression = "(>^.^)>";
			break;
		//crying
		case 4:
			expression = "(>;.;)>";
			break;
		//confused
		case 5:
			expression = "?(>'_')>";
			break;
	}
}

string Kirb::displayExpression()
{
	return expression;
}

string Kirb::displayName()
{
	return name;
}
