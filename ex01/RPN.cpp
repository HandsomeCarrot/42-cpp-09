#include "RPN.hpp"
#include <cctype>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
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
	_stack = t_rpn_stack();

	if (expression.empty())
		throw std::runtime_error("empty expression");

	std::istringstream	stream(expression);
	std::string			token;

	while (stream >> token)
	{
		if (token.length() > 1)
			throw std::runtime_error("unexpected token '" + token + "': token too long");
		
		if (std::isdigit(static_cast<unsigned char>(token[0])))
			_stack.push(token[0] - '0');
		else
		{
			if (_stack.size() < 2)
				throw std::runtime_error("unexpected token '" + token + "': expected a digit");

			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();
			
			int r = 0;

			switch (token[0])
			{
				case '+':
					r = a + b;
					if (r < a)
						throw std::runtime_error("addition: integer overflow");
					_stack.push(r);
					continue ;
				case '-':
					r = a - b;
					if (r > a)
						throw std::runtime_error("subtraction: integer underflow");
					_stack.push(r);
					continue ;
				case '/':
					if (b == 0)
						throw std::runtime_error("division by 0");
					_stack.push(a / b);
					continue ;
				case '*':
					r = a * b;
					if (r < a)
						throw std::runtime_error("multiplication: integer overflow");
					_stack.push(r);
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
