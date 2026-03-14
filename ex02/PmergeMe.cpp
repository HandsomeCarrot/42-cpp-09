#include "PmergeMe.hpp"
#include <cerrno>		//errno, ERANGE
#include <cstdlib>		//std::strtol
#include <iostream>		//std::cerr/cout/endl
#include <limits>		//std::numeric_limits
#include <sstream>		//std::istringstream
#include <stdexcept>	//std::runtime_error

/**
 * @brief default constructor
*/
PmergeMe::PmergeMe(void) :
	_vector_sorted(false),
	_deque_sorted(false)
{
	DEBUG_MSG("PmergeMe default constructor called");
}

/**
 * @brief copy constructor
 * 
 * @param other object to copy
*/
PmergeMe::PmergeMe(const PmergeMe &other)
{
	DEBUG_MSG("PmergeMe copy constructor called");
	this->_vector_container = other._vector_container;
	this->_vector_sorted = other._vector_sorted;
	this->_deque_container = other._deque_container;
	this->_deque_sorted = other._deque_sorted;
}

/**
 * @brief destructor
*/
PmergeMe::~PmergeMe(void)
{
	DEBUG_MSG("PmergeMe destructor called");
}

/**
 * @brief assignment operator
 * 
 * @param other object to assign from
 * 
 * @return reference to 'this' object
*/
PmergeMe	&PmergeMe::operator=(const PmergeMe &other)
{
	DEBUG_MSG("PmergeMe assignment operator called");
	if (this != &other)
	{
		this->_vector_container = other._vector_container;
		this->_vector_sorted = other._vector_sorted;
		this->_deque_container = other._deque_container;
		this->_deque_sorted = other._deque_sorted;
	}
	return (*this);
}

namespace
{
	/**
	 * @brief Converts a single token into a non-negative int value.
	 *
	 * @param token string token extracted from the input sequence
	 *
	 * @return the parsed integer value
	 *
	 * @throws std::runtime_error if @p token contains non-numeric characters,
	 * represents a value outside the `int` range, or represents a negative
	 * number
	 */
	int	parseValueToken(const std::string & token)
	{
		char * end = NULL;
		long parsed_value;

		errno = 0;
		parsed_value = std::strtol(token.c_str(), &end, 10);

		if (end == token.c_str() || *end != '\0')
			throw std::runtime_error("non-numeric character encountered: " + token);
		else if (errno == ERANGE
			|| parsed_value < 0
			|| parsed_value > std::numeric_limits<int>::max())
			throw std::runtime_error("number out of range: " + token);

		return (static_cast<int>(parsed_value));
	}
}

/**
 * @brief parameterized constructor
 * 
 * Parses a whitespace-separated sequence of non-negative integers into both
 * internal containers while preserving the input order.
 *
 * The constructor leaves both sorted-status flags set to false.
 *
 * @param value_sequence string containing the values to store
 * 
 * @throws std::runtime_error
 *   - if @p value_sequence
 *     - is empty
 *     - contains only whitespaces
 *     - contains a token with non-numeric characters
 *     - contains a value outside the `int` range
 *     - contains a negative number
 */
PmergeMe::PmergeMe(const std::string & value_sequence) :
	_vector_sorted(false),
	_deque_sorted(false)
{
	DEBUG_MSG("PmergeMe parameterized constructor called");

	std::istringstream	token_stream(value_sequence);
	std::string			token;

	while (token_stream >> token)
	{
		int	value = parseValueToken(token);

		DEBUG_MSG("pushed: " << value);

		_vector_container.push_back(value);
		_deque_container.push_back(value);
	}

	if (_vector_container.empty())
		throw std::runtime_error("sequence is empty");
}

const PmergeMe::t_vector PmergeMe::getVectorContainer(void) const
{
	return (_vector_container);
}

const PmergeMe::t_deque PmergeMe::getDequeContainer(void) const
{
	return (_deque_container);
}

bool PmergeMe::getVectorSortedStatus(void) const
{
	return (_vector_sorted);
}

bool PmergeMe::getDequeSortedStatus(void) const
{
	return (_deque_sorted);
}

/** 
 * @brief output stream operator
 * 
 * @param os reference to the outputstream
 * @param class reference to the class object
 * 
 * @return reference to the output stream
*/
std::ostream	&operator<<(std::ostream &os, const PmergeMe &c)
{
	//TODO
	(void)c;
	return (os);
}

namespace
{
	void switchPair(PmergeMe::t_vector & v, PmergeMe::t_vector::size_type a_index, PmergeMe::t_vector::size_type step)
	{
		
	}

	void sortPairs(PmergeMe::t_vector & v, PmergeMe::t_vector::size_type step)
	{
		for (PmergeMe::t_vector::size_type i = 0; i < v.size(); i += step)
		{
			if (i + 1 > v.size())
				break ;
			
			PmergeMe::t_vector::size_type left_pair_node = i * step;
			PmergeMe::t_vector::size_type right_pair_node = i * step + step;

			if (v[left_pair_node] < v[right_pair_node])
			{
				//move pairs (whole blocks)
			}
		}
	}
}

void PmergeMe::sort(t_vector & v, t_vector::size_type step)
{
	if (step == 0)
		throw std::runtime_error("vector sort: step of 0 is invalid");

	if (step >= v.size()) //return if one value remaining
		return ;

	//sort the pairs
	// for (t_vector::size_type i = 0; i < v_size; i += step)
	// {
	// 	if (i + 1 > v_size) //check hanging value
	// 		break ;

	// 	if (v[i] < v[i+1])
	// }
}
