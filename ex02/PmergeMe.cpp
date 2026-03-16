#include "PmergeMe.hpp"
#include <algorithm>	//std::swap_ranges
#include <cerrno>		//errno, ERANGE
#include <cmath>		//std::pow
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

void PmergeMe::switchPair(t_vector & v, t_vector::size_type index, t_vector::size_type step)
{
	t_vector::iterator i1 = v.begin() + index;
	t_vector::iterator i2 = v.begin() + index + step;

	std::swap_ranges(i1, i2, i2);
}

void PmergeMe::sortPairs(t_vector & v, t_vector::size_type step)
{
	DEBUG_MSG("SORTING: step size = " << step);

	for (t_vector::size_type block = 0; block + (2 * step) <= v.size(); block += (2 * step))
	{
		t_vector::size_type left = block + step - 1;
		t_vector::size_type right = left + step;

		DEBUG_MSG("index = " << block << ": pair = " << v[left] << " | " << v[right]);

		if (v[left] > v[right])
		{
			switchPair(v, block, step);
			DEBUG_MSG("pair swapped -> " << v[left] << " | " << v[right]);
		}
	}
}

// odd vector size not implemented
void PmergeMe::sort(t_vector & v, t_vector::size_type step)
{
	if (step == 0)
		throw std::runtime_error("vector sort: step of 0 is invalid");

	if (step >= (v.size() / 2)) //return if one value remaining
		return ;

	sortPairs(v, step);

	sort(v, step * 2);

	DEBUG_MSG("INSERTING: step size = " << step);
	
	t_vector::size_type k = 2;
	bool				end = false;
	bool				odd = v.size() % 2;

	while (!end)
	{
		t_vector::size_type insert_start = (std::pow(2, (k + 1)) + std::pow(-1, k)) / 3;
		t_vector::size_type insert_end = ((std::pow(2, (k)) + std::pow(-1, (k - 1))) / 3) + 1;

		DEBUG_MSG("k = " << k << ", insert start = " << insert_start << ", insert end = " << insert_end);

		t_vector::size_type start_index = 2 * insert_start * step - 1;
		t_vector::size_type end_index = 2 * insert_end * step - 1;
		
		if (odd)
		{
			DEBUG_MSG("correcting: odd");
			start_index -= step;
			end_index -= step;
		}
		
		if (end_index >= v.size())
			break ;
		
		DEBUG_MSG("start index = " << start_index << ", end index = " << end_index);

		//mark the end the loop
		if (start_index + step >= v.size())
		{
			end = true;
			DEBUG_MSG("end");
		}

		// go to first valid index
		while (start_index >= v.size())
		{
			start_index -= (2 * step);
			DEBUG_MSG("correcting: start index = " << start_index);
		}

		if (start_index < end_index)
			break ;

		while (start_index >= end_index)
		{
			DEBUG_MSG("insert: " << v[start_index]);
			start_index -= (2 * step);
		}

		++k;
	}
}

void PmergeMe::sort(void)
{
	DEBUG_MSG("vector before: " << containerToString(getVectorContainer(), 0));
	// add timer
	sort(_vector_container);
	DEBUG_MSG("vector after : " << containerToString(getVectorContainer(), 0));
	// sort other container
}
