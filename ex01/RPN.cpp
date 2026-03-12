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
	int checkedResult(long long value)
	{
		if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
			throw std::runtime_error("result out of bounds");

		return (static_cast<int>(value));
	}

	int applyOperator(char op, int a, int b)
	{
		DEBUG_MSG("op: " << a << ' ' << op << ' ' << b);
		switch (op)
		{
			case '+':
				return (checkedResult(static_cast<long long>(a) + b));
			case '-':
				return (checkedResult(static_cast<long long>(a) - b));
			case '*':
				return (checkedResult(static_cast<long long>(a) * b));
			case '/':
			{
				if (b == 0)
					throw std::runtime_error("division by zero");
				return (checkedResult(static_cast<long long>(a) / b));
			}
			default:
				throw std::runtime_error(std::string("'") + op + "': invalid operator");
		}
	}
}

void RPN::processOperand(char c)
{
	_stack.push(c - '0');
	DEBUG_MSG("push: " << (c - '0'));
}

void RPN::processOperator(const std::string & token)
{
	char op = token[0];

	switch (op)
	{
		case '+':
		case '-':
		case '*':
		case '/':
		{
			if (_stack.size() < 2)
				throw std::runtime_error("'" + token + "': need 2 operands");

			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();
			int result = applyOperator(op, a, b);

			_stack.push(result);
			DEBUG_MSG("= " << result);
			break ;
		}
		default:
			throw std::runtime_error("'" + token + "': invalid operator");
	}
}

int RPN::evaluate(const std::string & expression)
{
	std::istringstream	expression_stream(expression);
	std::string			token;
	bool				has_tokens = false;

	_stack = t_rpn_stack();

	while (expression_stream >> token)
	{
		has_tokens = true;

		if (token.length() > 1)
			throw std::runtime_error("'" + token + "': multi-char token");

		if (std::isdigit(static_cast<unsigned char>(token[0])))
			processOperand(token[0]);
		else
			processOperator(token);
	}

	if (!has_tokens)
		throw std::runtime_error("empty expression");
	if (_stack.size() != 1)
		throw std::runtime_error("too many operands: missing operator");

	DEBUG_MSG("=> " << _stack.top());
	return (_stack.top());
}
