#include "RPN.hpp"
#include <exception>	//std::exception
#include <iostream>		//std::cerr
#include <stdexcept>	//std::runtime_error
#include <string>		//std::string

namespace
{
	std::string parseInput(int argc, char ** argv)
	{
		if (argc != 2)
			throw std::runtime_error("invalid usage: expecetd exactly one argument");

		std::string input = argv[1];

		if (input.empty())
			throw std::runtime_error("invalid input: empty argument");

		return (input);
	}
}

int main(int argc, char ** argv)
{
	try
	{
		std::string input;

		input = parseInput(argc, argv);

		RPN rpn;
		
		std::cout << rpn.evaluate(input) << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}