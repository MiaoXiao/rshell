#include <iostream>
#include "Kirb.h"

using namespace std;

void Kirb::feedFood(string food)
{
	
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
	}
}