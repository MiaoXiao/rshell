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
		bool enable;

	public:
		Kirb()
			:name("Kirb"), expression("(>'.')>"), kirbstatus(1), enable(true){}
		//select a kirb command
		void selectCommand(char* argumentList[], int &status);
		//feeding specific food changes Kirb's expression and gives other special effects
		void feedFood(std::string food);
		//Change name
		void changeName(std::string name);
		//Enable or disable Kirb
		void dekirb();
		
		//display Kirb's expression
		std::string displayExpression();
		//display name
		std::string displayName();
		//show whether kirb is enabled/disabled
		bool showStatus();

		//change Kirb's expression
		void changeExpression(unsigned int face);
};

#endif 
