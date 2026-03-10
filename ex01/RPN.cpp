#include "RPN.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

/**
 * @brief default constructor
*/
RPN::RPN(void)
{
	DEBUG_MSG("RPN default constructor called");
}

/**
 * @brief copy constructor
 *
 * @param other object to copy
*/
RPN::RPN(const RPN & other) :
	_stack(other._stack)
{
	DEBUG_MSG("RPN copy constructor called");
	(void)other;
}

/**
 * @brief destructor
*/
RPN::~RPN(void)
{
	DEBUG_MSG("RPN destructor called");
}

/**
 * @brief assignment operator
 *
 * @param other object to assign from
 *
 * @return reference to 'this' object
*/
RPN & RPN::operator=(const RPN & other)
{
	DEBUG_MSG("RPN assignment operator called");
	if (this != &other)
	{
		_stack = other._stack;
	}
	return (*this);
}

int RPN::evaluate(const std::string & expression)
{
	if (expression.empty())
		throw std::runtime_error("empty expression");

	std::istringstream	stream(expression);
	std::string			token;

	while (stream >> token)
	{
		if (token.length() > 1)
			throw std::runtime_error("encountered unexpected token '" + token + "'");
		std::cout << token << std::endl;
	}

	return (0);
}
