#ifndef KIRB_H
#define KIRB_H

#include <string>
#include <vector>

class Kirb
{
		std::string name;
		std::string expression;
		std::vector<std::string> sv;
		int kirbstatus;

	public:
		Kirb()
			:name("Kirb"), expression("(>'.')>"), kirbstatus(1){}
		//select a kirb command
		void selectCommand(char* argumentList[], int &status);
		//feeding specific food changes Kirb's expression and gives other special effects
		void feedFood(std::string food);
		//Change name
		void changeName(std::string name);

		//display Kirb's expression
		std::string displayExpression();
		//display name
		std::string displayName();

		//change Kirb's expression
		void changeExpression(unsigned int face);

};

#endif 
