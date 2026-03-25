#include "PmergeMe.hpp"
#include <algorithm>	//std::swap_ranges
#include <cerrno>		//errno, ERANGE
#include <cmath>		//std::pow
#include <cstdlib>		//std::strtol
#include <iostream>		//std::cerr/cout/endl
#include <limits>		//std::numeric_limits
#include <sstream>		//std::istringstream
#include <stdexcept>	//std::runtime_error
#include <vector>

/**
 * @brief default constructor
*/
PmergeMe::PmergeMe(void) :
	_vector_sorted(false),
	_vector_comparison_count(0),
	_deque_sorted(false),
	_deque_comparison_count(0)
{
	DEBUG_MSG_LABEL(0, "[PmergeMe] ", "constructor(default)");
}

/**
 * @brief copy constructor
 * 
 * @param other object to copy
*/
PmergeMe::PmergeMe(const PmergeMe &other)
{
	DEBUG_MSG_LABEL(0, "[PmergeMe] ", "constructor(copy)");

	this->_vector_container = other._vector_container;
	this->_vector_sorted = other._vector_sorted;
	this->_vector_comparison_count = other._vector_comparison_count;
	this->_deque_container = other._deque_container;
	this->_deque_sorted = other._deque_sorted;
	this->_deque_comparison_count = other._deque_comparison_count;
}

/**
 * @brief destructor
*/
PmergeMe::~PmergeMe(void)
{
	DEBUG_MSG_LABEL(0, "[PmergeMe] ", "deconstructor");
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
	DEBUG_MSG_LABEL(0, "[PmergeMe] ", "operator=");
	if (this != &other)
	{
		this->_vector_container = other._vector_container;
		this->_vector_sorted = other._vector_sorted;
		this->_vector_comparison_count = other._vector_comparison_count;
		this->_deque_container = other._deque_container;
		this->_deque_sorted = other._deque_sorted;
		this->_deque_comparison_count = other._deque_comparison_count;
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
	_vector_comparison_count(0),
	_deque_sorted(false),
	_deque_comparison_count(0)
{
	DEBUG_MSG_LABEL(0, "[PmergeMe] ", "constructor(param)");

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

unsigned int PmergeMe::getVectorComparisonCount(void) const
{
	return (_vector_comparison_count);
}

unsigned int PmergeMe::getDequeComparisonCount(void) const
{
	return (_deque_comparison_count);
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

/**
 * @brief Compares two values while tracking the number of comparisons made.
 *
 * Increments @p comparison_counter before returning the arithmetic difference
 * between @p value1 and @p value2. If the counter has already reached its
 * maximum representable value, a warning is printed before the increment,
 * meaning the next increment wraps according to `unsigned int` semantics.
 *
 * @param value1 first value to compare
 * @param value2 second value to compare
 * @param comparison_counter reference to the comparison counter to update
 *
 * @return a positive value if @p value1 is greater than @p value2, zero if
 * they are equal, or a negative value otherwise
 */
long long PmergeMe::compare(int value1, int value2, unsigned int & comparison_counter, int debug_msg_indent_lvl)
{
	if (comparison_counter == std::numeric_limits<unsigned int>::max())
		std::cerr << "Warning: comparison count overflowed!" << std::endl;

	++comparison_counter;

	DEBUG_MSG(debug_msg_indent_lvl, "cmp(" << comparison_counter << ") -> [" << value1 << " | " << value2 << "]");
	(void)debug_msg_indent_lvl;

	return (value1 - value2);
}

void PmergeMe::switchPair(t_vector & values, t_vector::size_type pair_start_index, t_vector::size_type block_size)
{
	t_vector::iterator left_block_begin = values.begin() + pair_start_index;
	t_vector::iterator right_block_begin = values.begin() + pair_start_index + block_size;

	std::swap_ranges(left_block_begin, right_block_begin, right_block_begin);
}

void PmergeMe::sortPairs(t_vector & values, t_vector::size_type block_size)
{
	DEBUG_MSG(1, "block size = " << block_size);
	for (t_vector::size_type pair_start_index = 0; pair_start_index + (2 * block_size) <= values.size(); pair_start_index += (2 * block_size))
	{
		t_vector::size_type left_block_last_index = pair_start_index + block_size - 1;
		t_vector::size_type right_block_last_index = left_block_last_index + block_size;

		if (compare(values[left_block_last_index], values[right_block_last_index], _vector_comparison_count, 2) > 0)
			switchPair(values, pair_start_index, block_size);
	}
	DEBUG_MSG_CONTAINER(1, "values after: ", values);
}

// maybe make this function simpler, so it takes less variables
void PmergeMe::insertPendingGroup(
	const t_vector & values,
	std::vector<t_vector::size_type> & index_list,
	t_vector::size_type block_size,
	t_vector::size_type group_lower_bound,
	t_vector::size_type group_upper_bound)
{
	t_vector::size_type current_group = group_upper_bound;

	while (current_group >= group_lower_bound)
	{
		t_vector::size_type current_insert_index = ((2 * current_group) - 1) * block_size - 1;

		DEBUG_MSG(3, "insert group " << current_group << ": v[" << current_insert_index << "] = " << values[current_insert_index]);

		int					current_value = values[current_insert_index];
		t_vector::size_type	partner_index = current_insert_index + block_size;
		t_vector::size_type	left = 0;
		t_vector::size_type	right = 0;

		if (partner_index >= values.size())
			right = index_list.size();
		else
		{
			while (right < index_list.size() && index_list[right] != partner_index)
				++right;
		}

		DEBUG_MSG(4, DIM << "binary search window [" << left << ", " << right << ")" << RESET);

		while (left < right)
		{
			t_vector::size_type mid = left + ((right - left) / 2);

			if (compare(current_value, values[index_list[mid]], _vector_comparison_count, 5) < 0)
				right = mid;
			else
				left = mid + 1;
		}

		index_list.insert(index_list.begin() + left, current_insert_index);
		DEBUG_MSG(4, "inserted at index_list[" << left << "]");

		--current_group;
	}
}

void PmergeMe::insertPendingBlocks(t_vector & values, t_vector::size_type block_size)
{
	t_vector::size_type group_upper_bound = 3;
	t_vector::size_type group_lower_bound = 2;
	t_vector::size_type	jacobsthal_index = 3;
	t_vector::size_type block_count = values.size() / block_size;
	t_vector::size_type pending_block_count = (block_count + 1) / 2;

	DEBUG_MSG(1, "block size = " << block_size << " | blocks = " << block_count << " | insertions = " << pending_block_count);
	
	if (pending_block_count <= 1)
	{
		DEBUG_MSG(2, DIM << "nothing changed" << RESET);
		return ;
	}

	std::vector<t_vector::size_type> index_list;
	index_list.reserve(block_count);

	//insert smaller value of first block pair
	if (block_count > 0)
		index_list.push_back(block_size - 1);

	//insert larger values of all block pairs
	for (t_vector::size_type block_number = 2; block_number <= block_count; block_number += 2)
		index_list.push_back((block_number * block_size) - 1);
	DEBUG_MSG_CONTAINER(1, "indexes before: ", index_list);

	while (group_lower_bound <= pending_block_count)
	{
		t_vector::size_type next_block_index = std::min(group_upper_bound, pending_block_count);
	
		DEBUG_MSG(2, "groups " << next_block_index << ".." << group_lower_bound);

		insertPendingGroup(values, index_list, block_size, group_lower_bound, next_block_index);

		group_lower_bound = group_upper_bound + 1;
		group_upper_bound = std::pow(2, jacobsthal_index) - group_upper_bound;
		++jacobsthal_index;
	}

	DEBUG_MSG_CONTAINER(1, "indexes after: ", index_list);

	t_vector sorted_values;
	sorted_values.reserve(values.size());

	for (t_vector::size_type i = 0; i < index_list.size(); ++i)
	{
		t_vector::size_type block_end = index_list[i];
		t_vector::size_type block_start = block_end - block_size + 1;

		for (t_vector::size_type j = block_start; j <= block_end; ++j)
			sorted_values.push_back(values[j]);
	}

	for (t_vector::size_type i = block_count * block_size; i < values.size(); ++i)
		sorted_values.push_back(values[i]);

	values = sorted_values;

	DEBUG_MSG_CONTAINER(1, "values after: ", values);
}

void PmergeMe::sort(t_vector & values)
{
	t_vector::size_type	block_size = 1;

	DEBUG_MSG(0, BOLD << "PAIR SORT" << RESET);
	while (block_size < (values.size() / 2))
	{
		sortPairs(values, block_size);
		block_size *= 2;
	}

	DEBUG_MSG(0, BOLD << "INSERT" << RESET);

	while (block_size > 1)
	{
		block_size /= 2;
		insertPendingBlocks(values, block_size);
	}
}

void PmergeMe::sort(void)
{
	DEBUG_HEADER("VECTOR");
	DEBUG_MSG_CONTAINER(0, "given values: ", _vector_container);
	// add timer
	sort(_vector_container);
	// sort other container
}
