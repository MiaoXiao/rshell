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

	switch (sv.size())
	{
		case(0):
			cout << "Error with Kirb: not enough arguments" << endl;
			status = 1;
			break;
		case(1):
			cout << "Error with Kirb: not enough arguments" << endl;
			status = 1;
			break;
		case(2):
			if (sv[0] == "feed")
				feedFood(sv[1]);
			else if (sv[0] == "changename")
				changeName(sv[1]);
			else
			{
				cout << "Error with Kirb: kirb function does not exist" << endl;
				status = 1;
			}
			break;
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
	
	kirbstatus = 0;
}

void Kirb::changeName(string newname)
{
	cout << name << "'s name is changed to ";
	name = newname; 
	cout << newname << "!" << endl;

	kirbstatus = 0;
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

void Kirb::dekirb()
{
	if (enable)
		cout << "Kirb disabled" << endl;
	else
		cout << "Kirb enabled" << endl;
	enable = !enable;
	kirbstatus = 0;
}

string Kirb::displayExpression()
{
	return expression;
}

string Kirb::displayName()
{
	return name;
}

bool Kirb::showStatus()
{
	return enable;
}
