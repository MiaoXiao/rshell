#ifndef KIRB_H
#define KIRB_H

#include <string>

class Kirb
{
	public:
		std::string expression;
		Kirb()
			:expression("(>'.')>"){}
		//feeding specific food changes Kirb's expression and gives other special effects
		void feedFood(std::string food);
	private:
		//change Kirb's expression
		void changeExpression(unsigned int face);

};

#endif 
