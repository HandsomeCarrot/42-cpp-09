#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <deque>
#include <ostream>	//std::ostream
#include <string>
#include <vector>

class PmergeMe
{
// ----- Variables ----- //
public: 
	typedef std::vector<int>	t_vector;
	typedef std::deque<int>		t_deque;
private:
	t_vector	_vector_container;
	bool		_vector_sorted;
	//timer result for vector

	t_deque	_deque_container;
	bool	_deque_sorted;
	//timer result for deque

// ----- De/Constructors ----- //
private:
	PmergeMe(void);
public:
	PmergeMe(const PmergeMe &other);
	~PmergeMe(void);

	PmergeMe	&operator=(const PmergeMe &other);

	PmergeMe(const std::string & value_sequence);

// ----- Getters ----- //
public:
	const t_vector	getVectorContainer(void) const;
	const t_deque	getDequeContainer(void) const;
	
	bool	getVectorSortedStatus(void) const;
	bool	getDequeSortedStatus(void) const;

	//TODO something	getVectorTimer(void) const;
	//TODO something	getDequeTimer(void) const;

// ----- Functions ----- //
public:
	void	sort(t_vector & v, std::size_t step = 1);
	//TODO void	sortDeque(std::deque<int> & v, std::size_t step = 1);
	void	sort(void);
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
