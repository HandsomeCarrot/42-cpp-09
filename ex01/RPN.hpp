#ifndef RPN_HPP
# define RPN_HPP

//# include <iostream> Uncomment this if you use the output stream operator

class RPN
{
private:
protected:
public:
	RPN(void);
	//RPN(<all parameters of class>);
	RPN(const RPN &other);
	~RPN(void);

	RPN	&operator=(const RPN &other);
};

//std::ostream	&operator<<(std::ostream &os, const RPN &c);

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* RPN_HPP */
