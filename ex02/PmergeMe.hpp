#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <deque>	//std::deque
#include <ostream>	//std::ostream
#include <sstream>	//std::ostringstream
#include <string>	//std::string
#include <vector>	//std::vector

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
private:
	static void	switchPair(t_vector & v, t_vector::size_type index, t_vector::size_type step);
	static void	sortPairs(t_vector & v, t_vector::size_type step);

public:
	void	sort(t_vector & v, std::size_t step = 1);
	//TODO void	sortDeque(std::deque<int> & v, std::size_t step = 1);
	void	sort(void);
};

std::ostream	&operator<<(std::ostream &os, const PmergeMe &c);

template <typename Container>
std::string	containerToString(const Container &container, std::size_t max_elements = 6)
{
	typename Container::const_iterator	it = container.begin();
	std::ostringstream					oss;
	std::size_t							count = 0;

	oss << "[ ";

	while (it != container.end())
	{
		if (count > 0)
			oss << " | ";

		if (max_elements > 0 && count >= max_elements)
		{
			oss << "...";
			break ;
		}

		oss << *it;

		++it;
		++count;
	}

	oss << " ]";

	return (oss.str());
}

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"
# define CYAN	"\033[36m"
# define BOLD	"\033[1m"
# define DIM	"\033[2m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG_LABEL_COLOR(color, label, msg) std::cerr << color << label << RESET << msg << std::endl
#  define DEBUG_MSG_LABEL(label, msg) DEBUG_MSG_LABEL_COLOR(YELLOW, label, msg)
#  define DEBUG_MSG(msg) DEBUG_MSG_LABEL("> ", msg)

#  define DEBUG_PHASE(description) std::cerr << std::endl << BOLD << CYAN << "══════ " << description << " ══════" << RESET << std::endl

#  define DEBUG_MSG_CONTAINER(msg, container) DEBUG_MSG_LABEL_COLOR(DIM, msg, containerToString(container, 0))
# else
#  define DEBUG_MSG_LABEL_COLOR(color, label, msg)
#  define DEBUG_MSG_LABEL(label, msg)
#  define DEBUG_MSG(msg)
#  define DEBUG_PHASE(description)
#  define DEBUG_MSG_CONTAINER(msg, container)
# endif

#endif /* PMERGEME_HPP */
