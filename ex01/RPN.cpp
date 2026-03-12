#include "RPN.hpp"
#include <cctype>
#include <limits>
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

namespace
{
	int checkedResult(long long value, const char * operation)
	{
		if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
			throw std::runtime_error(std::string(operation) + ": result out of range");
		return (static_cast<int>(value));
	}

	int applyOperator(char op, int a, int b)
	{
		switch (op)
		{
			case '+':
				return (checkedResult(static_cast<long long>(a) + b, "addition"));
			case '-':
				return (checkedResult(static_cast<long long>(a) - b, "subtraction"));
			case '*':
				return (checkedResult(static_cast<long long>(a) * b, "multiplication"));
			case '/':
				if (b == 0)
					throw std::runtime_error("division by 0");
				if (a == std::numeric_limits<int>::min() && b == -1)
					throw std::runtime_error("division: result out of range");
				return (a / b);
			default:
				throw std::runtime_error(std::string("unexpected token '") + op + "': invalid operator");
		}
	}
}

int RPN::evaluate(const std::string & expression)
{
	_stack = t_rpn_stack();

	if (expression.empty())
		throw std::runtime_error("empty expression");

	std::istringstream	stream(expression);
	std::string			token;
	bool				hasTokens = false;

	while (stream >> token)
	{
		hasTokens = true;

		if (token.length() > 1)
			throw std::runtime_error("unexpected token '" + token + "': token too long");
		
		if (std::isdigit(static_cast<unsigned char>(token[0])))
			_stack.push(token[0] - '0');
		else
		{
			switch (token[0])
			{
				case '+':
				case '-':
				case '*':
				case '/':
				{
					if (_stack.size() < 2)
						throw std::runtime_error("unexpected token '" + token + "': not enough operands");

					int b = _stack.top();
					_stack.pop();
					int a = _stack.top();
					_stack.pop();
					_stack.push(applyOperator(token[0], a, b));

					continue ;
				}
				default:
					throw std::runtime_error("unexpected token '" + token + "': invalid operator");
			}
		}
	}

	if (!hasTokens)
		throw std::runtime_error("empty expression");

	if (_stack.size() != 1)
		throw std::runtime_error("too many operands");

	return (_stack.top());
}
