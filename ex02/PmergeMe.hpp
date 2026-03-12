#ifndef PMERGEME_HPP
# define PMERGEME_HPP

//# include <iostream> Uncomment this if you use the output stream operator

class PmergeMe
{
private:
protected:
public:
	PmergeMe(void);
	//PmergeMe(<all parameters of class>);
	PmergeMe(const PmergeMe &other);
	~PmergeMe(void);

	PmergeMe	&operator=(const PmergeMe &other);
};

//std::ostream	&operator<<(std::ostream &os, const PmergeMe &c);

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* PMERGEME_HPP */
