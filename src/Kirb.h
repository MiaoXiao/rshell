#ifndef KIRB_H
#define KIRB_H

#include <string>

class Kirb
{
		std::string name;
		std::string expression;
	public:
		Kirb()
			:expression("(>'.')>"), name("Kirb"){}
		//select a kirb command
		void selectCommand(char* argumentList[], int &status);
		//feeding specific food changes Kirb's expression and gives other special effects
		void feedFood(std::string food);
		//display Kirb's expression
		std::string displayExpression();
		//display name
		std::string displayName();
		//Change name
		void changeName(std::string name);
	private:
		//change Kirb's expression
		void changeExpression(unsigned int face);

};

#endif 
