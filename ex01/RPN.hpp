#ifndef RPN_HPP
# define RPN_HPP

#include <list>
#include <stack>
#include <string>

class RPN
{
private:
	typedef std::stack<int, std::list<int> > t_rpn_stack;

	t_rpn_stack _stack;

	void	processOperand(char c);
	void	processOperator(const std::string & token);
public:
	RPN(void);
	RPN(const RPN & other);
	~RPN(void);

	RPN	& operator=(const RPN & other);

	int	evaluate(const std::string & expression);
};

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* RPN_HPP */
