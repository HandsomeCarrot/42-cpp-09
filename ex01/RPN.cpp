#include "RPN.hpp"
#include <cctype>
#include <cstddef>
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
	std::string buildTokenMessage(std::size_t tokenIndex, const std::string & message)
	{
		std::ostringstream stream;

		stream << "token " << tokenIndex << ": " << message;
		return (stream.str());
	}

	int checkedResult(long long value, const char * operation, std::size_t tokenIndex)
	{
		if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
			throw std::runtime_error(buildTokenMessage(tokenIndex, std::string(operation) + " overflow"));

		return (static_cast<int>(value));
	}

	int applyOperator(char op, int a, int b, std::size_t tokenIndex)
	{
		DEBUG_MSG("op: " << a << ' ' << op << ' ' << b);
		switch (op)
		{
			case '+':
				return (checkedResult(static_cast<long long>(a) + b, "addition", tokenIndex));
			case '-':
				return (checkedResult(static_cast<long long>(a) - b, "subtraction", tokenIndex));
			case '*':
				return (checkedResult(static_cast<long long>(a) * b, "multiplication", tokenIndex));
			case '/':
			{
				if (b == 0)
					throw std::runtime_error(buildTokenMessage(tokenIndex, "division by zero"));
				return (checkedResult(static_cast<long long>(a) / b, "division", tokenIndex));
			}
			default:
				throw std::runtime_error(buildTokenMessage(tokenIndex, std::string("'") + op + "': invalid operator"));
		}
	}
}

void RPN::processOperand(char c)
{
	_stack.push(c - '0');
	DEBUG_MSG("push: " << (c - '0'));
}

void RPN::processOperator(const std::string & token, std::size_t tokenIndex)
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
				throw std::runtime_error(buildTokenMessage(tokenIndex, "'" + token + "': need 2 operands"));

			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();
			int result = applyOperator(op, a, b, tokenIndex);

			_stack.push(result);
			DEBUG_MSG("= " << result);
			break ;
		}
		default:
			throw std::runtime_error(buildTokenMessage(tokenIndex, "'" + token + "': invalid operator"));
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
	std::size_t			tokenIndex = 0;

	while (stream >> token)
	{
		hasTokens = true;
		++tokenIndex;

		if (token.length() > 1)
			throw std::runtime_error(buildTokenMessage(tokenIndex, "'" + token + "': multi-char token"));

		if (std::isdigit(static_cast<unsigned char>(token[0])))
			processOperand(token[0]);
		else
			processOperator(token, tokenIndex);
	}

	if (!hasTokens)
		throw std::runtime_error("empty expression");
	if (_stack.size() != 1)
		throw std::runtime_error("too many operands: missing operator");

	DEBUG_MSG("=> " << _stack.top());
	return (_stack.top());
}
