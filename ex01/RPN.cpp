#include "RPN.hpp"
#include <cctype>
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
			throw std::runtime_error("unexpected token '" + token + "': token too long");
		
		if (std::isdigit(token[0]))
			_stack.push(token[0] - '0');
		else
		{
			if (_stack.size() < 2)
				throw std::runtime_error("unexpected token '" + token + "': expected a digit");

			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();

			switch (token[0])
			{
				case '+':
					_stack.push(a + b);
					continue ;
				case '-':
					_stack.push(a - b);
					continue ;
				case '/':
					_stack.push(a / b);
					continue ;
				case '*':
					_stack.push(a * b);
					continue ;
				default:
					throw std::runtime_error("unexpected token '" + token + "': invalid operator");
			}
		}
	}

	if (_stack.size() != 1)
		throw std::runtime_error("not enough operations");

	return (_stack.top());
}
