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
	DEBUG_MSG_LABEL("[PmergeMe] ", "ctor(default)");
}

/**
 * @brief copy constructor
 * 
 * @param other object to copy
*/
PmergeMe::PmergeMe(const PmergeMe &other)
{
	DEBUG_MSG_LABEL("[PmergeMe] ", "ctor(copy)");

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
	DEBUG_MSG_LABEL("[PmergeMe] ", "dtor");
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
	DEBUG_MSG_LABEL("[PmergeMe] ", "operator=");
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
		char * parse_end = NULL;
		long parsed_value;

		errno = 0;
		parsed_value = std::strtol(token.c_str(), &parse_end, 10);

		if (parse_end == token.c_str() || *parse_end != '\0')
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
	DEBUG_MSG_LABEL("[PmergeMe] ", "ctor(param)");

	std::istringstream	token_stream(value_sequence);
	std::string			token;

	while (token_stream >> token)
	{
		int	parsed_value = parseValueToken(token);

		_vector_container.push_back(parsed_value);
		_deque_container.push_back(parsed_value);
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

void PmergeMe::switchPair(t_vector & values, t_vector::size_type pair_start_index, t_vector::size_type block_size)
{
	t_vector::iterator left_block_begin = values.begin() + pair_start_index;
	t_vector::iterator right_block_begin = values.begin() + pair_start_index + block_size;

	std::swap_ranges(left_block_begin, right_block_begin, right_block_begin);
}

void PmergeMe::sortPairs(t_vector & values, t_vector::size_type block_size)
{
	for (t_vector::size_type pair_start_index = 0; pair_start_index + (2 * block_size) <= values.size(); pair_start_index += (2 * block_size))
	{
		t_vector::size_type left_block_last_index = pair_start_index + block_size - 1;
		t_vector::size_type right_block_last_index = left_block_last_index + block_size;

		if (values[left_block_last_index] > values[right_block_last_index])
		{
			switchPair(values, pair_start_index, block_size);
			DEBUG_MSG("cmp [" << values[right_block_last_index] << " | " << values[left_block_last_index] << "] -> swap");
		}
		else
		{
			DEBUG_MSG("cmp [" << values[left_block_last_index] << " | " << values[right_block_last_index] << "] -> no swap");
		}
	}
}

// odd vector size not implemented
void PmergeMe::sort(t_vector & values, t_vector::size_type block_size)
{
	if (block_size == 0)
		throw std::runtime_error("vector sort: step of 0 is invalid");

	if (block_size >= (values.size() / 2)) //return if one value remaining
		return ;

	DEBUG_PHASE("SORT | block size = " << block_size);

	sortPairs(values, block_size);

	DEBUG_MSG_CONTAINER("after: ", values);

	sort(values, block_size * 2);

	DEBUG_PHASE("INSERT | block size = " << block_size);

	t_vector::size_type group_upper_bound = 3;
	t_vector::size_type group_lower_bound = 2;
	t_vector::size_type	jacobsthal_index = 3;

	t_vector::size_type pending_block_count = ((values.size() / block_size) + 1) / 2;

	DEBUG_MSG("blocks = " << values.size() / block_size << " | insertions = " << pending_block_count);

	while (group_lower_bound <=pending_block_count)
	{
		DEBUG_MSG("groups " << group_upper_bound << ".." << group_lower_bound);

		t_vector::size_type pending_block_index = std::min(group_upper_bound, pending_block_count);

		while (pending_block_index >= group_lower_bound)
		{
			t_vector::size_type current_insert_index = ((2 * pending_block_index) - 1) * block_size - 1;

			DEBUG_MSG_LABEL("  > ", "insert group " << pending_block_count << ": v[" << current_insert_index << "] = " << values[current_insert_index]);

			--pending_block_index;
		}

		group_lower_bound = group_upper_bound + 1;
		group_upper_bound = std::pow(2, jacobsthal_index) - group_upper_bound;
		++jacobsthal_index;
	}

	DEBUG_MSG_CONTAINER("after: ", values);
}

void PmergeMe::sort(void)
{
	DEBUG_PHASE("VECTOR");
	DEBUG_MSG_CONTAINER("given: ", _vector_container);
	// add timer
	sort(_vector_container);
	// sort other container
}
