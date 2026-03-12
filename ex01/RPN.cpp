#include "RPN.hpp"
#include <cctype>		// std::isdigit()
#include <limits>		// std::numeric_limits<int>::min/max()
#include <sstream>		// std::istringstream
#include <string>		// std::string
#include <stdexcept>	// std::runtime_error

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
	/**
	 * @brief check long long calculation result for int overflow
	 *
	 * Operations are performed in `long long` first to detect overflow before the
	 * value is converted back to an 'int'.
	 *
	 * @param value, arithmetic result to validate.
	 *
	 * @return The validated result converted to `int`.
	 *
	 * @throws std::runtime_error If @p value is outside the `int` range.
	 */
	int checkedResult(long long value)
	{
		if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
			throw std::runtime_error("result out of bounds");

		return (static_cast<int>(value));
	}

	/**
	 * @brief Apply a binary arithmetic operator to two operands.
	 *
	 * The operands are evaluated in the order `a op b`, which matches the order
	 * in which they are popped from the RPN stack. The final result is validated
	 * with `checkedResult()` before it is returned.
	 *
	 * @param op Arithmetic operator to apply.
	 * @param a Left-hand operand.
	 * @param b Right-hand operand.
	 *
	 * @return The computed result.
	 *
	 * @throws std::runtime_error, If @p op is unsupported, if division by zero is
	 *         attempted, or if the result overflows `int`.
	 */
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

/**
 * @brief Push a single-digit operand onto the evaluation stack.
 *
 * The character is expected to be an ASCII decimal digit. Its numeric value is
 * obtained by subtracting `'0'` and then stored on the internal stack.
 *
 * @param c Digit character representing the operand to push.
 */
void RPN::processOperand(char c)
{
	_stack.push(c - '0');
	DEBUG_MSG("push: " << (c - '0'));
}

/**
 * @brief Consume an operator token and apply it to the top stack operands.
 *
 * This function accepts only four operator types (+, -, *, /). It
 * pops the two topmost operands, computes the result, and pushes that result
 * back onto the stack.
 *
 * @param token Single-character token containing the operator.
 *
 * @throws std::runtime_error If the operator is invalid or if fewer than two
 *         operands are available on the stack.
 */
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

/**
 * @brief Evaluate a whitespace-separated reverse Polish notation expression.
 *
 * The expression is read token by token. Single-digit numeric tokens are pushed
 * as operands, while operator tokens trigger a stack reduction. When parsing
 * finishes, exactly one value must remain on the stack for the expression to be
 * considered valid.
 *
 * @param expression Expression to evaluate.
 *
 * @return The final result left on the stack.
 *
 * @throws std::runtime_error, If the expression is empty, contains invalid or
 *         multi-character tokens, performs an invalid operation, or leaves an
 *         incorrect number of operands on the stack.
 */
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
