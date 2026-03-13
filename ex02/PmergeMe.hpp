#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <deque>
#include <ostream>	//std::ostream
#include <string>
#include <vector>

class PmergeMe
{
// ----- Variables ----- //
private:
	std::vector<int>	_vector_container;
	bool				_vector_sorted;
	//timer result for vector

	std::deque<int>	_deque_container;
	bool			_deque_sorted;
	//timer result for deque

// ----- De/Constructors ----- //
private:
	PmergeMe(void);
public:
	PmergeMe(const PmergeMe &other);
	~PmergeMe(void);

	PmergeMe	&operator=(const PmergeMe &other);

	PmergeMe(const std::string & value_string);

// ----- Getters ----- //
public:
	const std::vector<int>	getVectorContainer(void) const;
	const std::deque<int>	getDequeContainer(void) const;
	
	bool	getVectorSortedStatus(void) const;
	bool	getDequeSortedStatus(void) const;

	//TODO something	getVectorTimer(void) const;
	//TODO something	getDequeTimer(void) const;

// ----- Functions ----- //
public:
	//TODO void	sort(void);
	//TODO void	sortVector(some offset);
	//TODO void	sortDeque(some offset);
};

std::ostream	&operator<<(std::ostream &os, const PmergeMe &c);

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* PMERGEME_HPP */
