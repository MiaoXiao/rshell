#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "Kirb.h"

using namespace std;

void Kirb::selectCommand(char* argumentList[], int &status)
{

}

void Kirb::feedFood(string food)
{
	if (food == "peppers" || food == "pepper")
		changeExpression(2);
	else if (food == "sugar" || food == "candy")
		changeExpression(3);
	else if (food == "waddledee")
		changeExpression(2);
	else
		changeExpression(5);
}

string Kirb::displayExpression()
{
	return expression;
}

string Kirb::displayName()
{
	return name;
}

void Kirb::changeName(string newname)
{
	int stringsize = newname.length();
	if (stringsize == 0)
		cout << "Type \"kirb changename NAME\" to change " << name << "'s name." << endl;
	else if (stringsize > 20)
		cout << name << " doesin't like long names! [New name should be 20 characters or less]" << endl;
	else if (newname == "-default")
	{
		cout << name << "'s name is now Kirb!";
		name = "Kirb";
	}
	else
	{
		cout << name << "'s name is now ";
		name = newname;
		cout << newname << "!" << endl;
	}
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
			expression = "(>'O')>";
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
