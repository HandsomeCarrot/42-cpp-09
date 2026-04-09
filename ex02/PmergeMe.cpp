// SECTION includes

	#include "PmergeMe.hpp"
	#include <algorithm>	//std::swap_ranges
	#include <cerrno>		//errno, ERANGE
	#include <cstdlib>		//std::strtol
	#include <ctime>		//std::clock, std::clock_t, CLOCKS_PER_SEC
	#include <iostream>		//std::cerr/cout/endl
	#include <limits>		//std::numeric_limits
	#include <sstream>		//std::istringstream
	#include <stdexcept>	//std::runtime_error
	#include <vector>		//std::vector
	
	#ifdef DEBUG
	# include <ios>			//std::boolalpha
	# include <cmath>		//std::log, std::ceil
	#endif /* DEBUG */

// END_SECTION includes

// SECTION constructors

	/**
	 * @brief default constructor
	*/
	PmergeMe::PmergeMe(void)
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
		(void)other;
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
		(void)other;
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
	 * Parses a whitespace-separated sequence of non-negative integers into all 3
	 * internal containers while preserving the input order.
	 *
	 * The constructor leaves the sorted-status flag set to false.
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
		_vector_comparison_count(0),
		_vector_timer(0),
		_deque_comparison_count(0),
		_deque_timer(0)
	{
		DEBUG_MSG_LABEL(0, "[PmergeMe] ", "constructor(param)");
	
		std::istringstream	token_stream(value_sequence);
		std::string			token;
	
		while (token_stream >> token)
		{
			int	parsed_value = parseValueToken(token);
	
			_unsorted_vector.push_back(parsed_value);
			_vector_container.push_back(parsed_value);
			_deque_container.push_back(parsed_value);
		}
	
		if (_vector_container.empty())
			throw std::runtime_error("sequence is empty");
	}

	PmergeMe::PmergeMe(int argc, char ** argv) :
		_vector_comparison_count(0),
		_vector_timer(0),
		_deque_comparison_count(0),
		_deque_timer(0)
	{
		DEBUG_MSG_LABEL(0, "[PmergeMe] ", "constructor(param)");

		if (argc < 2)
			throw std::runtime_error("sequence is too small (minimum of 2 numbers)");

		for (int i = 0; i < argc; ++i)
		{
			int parsed_value = parseValueToken(argv[i]);

			_unsorted_vector.push_back(parsed_value);
			_vector_container.push_back(parsed_value);
			_deque_container.push_back(parsed_value);
		}
	}

// END_SECTION constructors

// SECTION accessors

	const PmergeMe::t_vector PmergeMe::getUnsortedVector(void) const
	{
		return (_unsorted_vector);
	}
	
	const PmergeMe::t_vector PmergeMe::getVectorContainer(void) const
	{
		return (_vector_container);
	}
	
	const PmergeMe::t_deque PmergeMe::getDequeContainer(void) const
	{
		return (_deque_container);
	}
	
	unsigned int PmergeMe::getVectorComparisonCount(void) const
	{
		return (_vector_comparison_count);
	}
	
	unsigned int PmergeMe::getDequeComparisonCount(void) const
	{
		return (_deque_comparison_count);
	}
	
	std::clock_t PmergeMe::getVectorTimer(void) const
	{
		return (_vector_timer);
	}
	
	std::clock_t PmergeMe::getDequeTimer(void) const
	{
		return (_deque_timer);
	}
	
	void PmergeMe::setVectorTimer(std::clock_t time)
	{
		_vector_timer = time;
	}
	
	void PmergeMe::getDequeTimer(std::clock_t time)
	{
		_deque_timer = time;
	}

// END_SECTION accessors

// SECTION stream operator

	namespace
	{
		#ifdef DEBUG
			unsigned int getMaxComparisons(unsigned int element_count)
			{
				unsigned int max_comparisons = 0;
				for (unsigned int k = 1; k <= element_count; ++k)
				{
					double val = (3.0 * k) / 4.0;
					double log2_val = std::log(val) / std::log(2.0);
					unsigned int term = static_cast<unsigned int>(std::ceil(log2_val));
					max_comparisons += term;
				}
				return (max_comparisons);
			}
		#endif /* DEBUG */
	
		std::string clockToString(std::clock_t ticks)
		{
			unsigned long long us = static_cast<unsigned long long>((static_cast<double>(ticks) * 1000000.0) / CLOCKS_PER_SEC);
	
			unsigned long long s = us / 1000000;
			us %= 1000000;
			unsigned long long ms = us / 1000;
			us %= 1000;
	
			std::ostringstream time_string;
			time_string << s << " s " << ms << " ms " << us << " us";
			return (time_string.str());
		}
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
		os << "Before: " << containerToString(c.getUnsortedVector()) << std::endl;
		os << "After : " << containerToString(c.getVectorContainer()) << std::endl;
	
		#ifdef DEBUG
			os << "After : " << containerToString(c.getDequeContainer()) << " (deque)" << std::endl;
			// os << "sorted: " << std::boolalpha << c.getSortedStatus() << std::endl;
		
			os << "comparisons: max = " \
				<< getMaxComparisons(c.getUnsortedVector().size()) \
				<< ", vector = " << c.getVectorComparisonCount() \
				<< ", deque = " \
				<< c.getDequeComparisonCount() \
				<< std::endl;
	
		#endif /* DEBUG */
	
		os << "Time to process a range of " \
			<< c.getUnsortedVector().size() \
			<< " elements with std::vector: " \
			<< clockToString(c.getVectorTimer()) \
			<< std::endl;
	
		os << "Time to process a range of " \
			<< c.getUnsortedVector().size() \
			<< " elements with std::deque : " \
			<< clockToString(c.getDequeTimer()) \
			<< std::endl;
	
		return (os);
	}

// END_SECTION stream operator

// SECTION sort helpers

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
	
	void PmergeMe::swapBlockPair(t_vector & values, t_vector::size_type pair_start_index, t_vector::size_type block_size)
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
				swapBlockPair(values, pair_start_index, block_size);
		}
		DEBUG_MSG_CONTAINER(1, "values after: ", values);
	}
	
	PmergeMe::t_vector::size_type PmergeMe::getBlockEndIndex(t_vector::size_type block_number, t_vector::size_type block_size) const
	{
		return ((block_number * block_size) - 1);
	}
	
	PmergeMe::t_vector::size_type PmergeMe::getBlockStartIndex(t_vector::size_type block_end_index, t_vector::size_type block_size) const
	{
		return (block_end_index - block_size + 1);
	}
	
	PmergeMe::t_vector::size_type PmergeMe::findPartnerInMainChain(
		const t_index_list & index_list,
		t_vector::size_type partner_index,
		t_vector::size_type value_count) const
	{
		t_vector::size_type partner_position = 0;
	
		if (partner_index >= value_count)
			return (index_list.size());
	
		while (partner_position < index_list.size() && index_list[partner_position] != partner_index)
			++partner_position;
	
		return (partner_position);
	}
	
	PmergeMe::t_vector::size_type PmergeMe::findInsertionPosition(
		const t_vector & values,
		const t_index_list & index_list,
		int current_value,
		t_vector::size_type right_bound)
	{
		t_vector::size_type left = 0;
		t_vector::size_type right = right_bound;
	
		DEBUG_MSG(4, DIM << "binary search window [" << left << ", " << right << ")" << RESET);
	
		while (left < right)
		{
			t_vector::size_type mid = left + ((right - left) / 2);
	
			if (compare(current_value, values[index_list[mid]], _vector_comparison_count, 5) < 0)
				right = mid;
			else
				left = mid + 1;
		}
	
		return (left);
	}
	
	PmergeMe::t_index_list PmergeMe::buildMainChainIndexList(
		t_vector::size_type block_count,
		t_vector::size_type block_size) const
	{
		t_index_list	index_list;
	
		index_list.reserve(block_count);
	
		if (block_count > 0)
			index_list.push_back(getBlockEndIndex(1, block_size));
	
		for (t_vector::size_type block_number = 2; block_number <= block_count; block_number += 2)
			index_list.push_back(getBlockEndIndex(block_number, block_size));
	
		return (index_list);
	}

	void PmergeMe::insertByJacobsthalOrder(
		const t_vector & values,
		t_index_list & index_list,
		t_vector::size_type block_size,
		t_vector::size_type pending_block_count)
	{
		// group_end: the current jacobsthal-number (a number of the jacobsthal-sequence),
		// but also the index for a (number)group in the vector
		t_vector::size_type group_start = 2;
		t_vector::size_type group_end = 3;
		t_vector::size_type prev_group_end = 1;
	
		while (group_start <= pending_block_count)
		{
			// cap to last block
			t_vector::size_type capped_group_end = std::min(group_end, pending_block_count);
	
			DEBUG_MSG(2, "groups " << capped_group_end << ".." << group_start);
	
			insertGroupRange(values, index_list, block_size, group_start, capped_group_end);
	
			group_start = group_end + 1;
			t_vector::size_type next_group_end = group_end + (2 * prev_group_end);
			prev_group_end = group_end;
			group_end = next_group_end;
		}
	}
	
	PmergeMe::t_vector PmergeMe::reorderByIndexList(
		const t_vector & values,
		const t_index_list & index_list,
		t_vector::size_type block_size,
		t_vector::size_type block_count) const
	{
		t_vector	sorted_values;
	
		sorted_values.reserve(values.size());
	
		for (t_vector::size_type i = 0; i < index_list.size(); ++i)
		{
			t_vector::size_type block_end = index_list[i];
			t_vector::size_type block_start = getBlockStartIndex(block_end, block_size);
	
			for (t_vector::size_type value_index = block_start; value_index <= block_end; ++value_index)
				sorted_values.push_back(values[value_index]);
		}
	
		for (t_vector::size_type i = block_count * block_size; i < values.size(); ++i)
			sorted_values.push_back(values[i]);
	
		return (sorted_values);
	}
	
	void PmergeMe::insertGroupRange(
		const t_vector & values,
		t_index_list & index_list,
		t_vector::size_type block_size,
		t_vector::size_type group_lower_bound,
		t_vector::size_type group_upper_bound)
	{
		for (t_vector::size_type current_group = group_upper_bound; current_group >= group_lower_bound; --current_group)
		{
			t_vector::size_type pending_block_number = (2 * current_group) - 1;
			t_vector::size_type pending_block_end = getBlockEndIndex(pending_block_number, block_size);
	
			DEBUG_MSG(3, "insert group " << current_group << ": v[" << pending_block_end << "] = " << values[pending_block_end]);
	
			int	current_value = values[pending_block_end];
			t_vector::size_type partner_index = pending_block_end + block_size;
			t_vector::size_type right_bound = findPartnerInMainChain(index_list, partner_index, values.size());
			t_vector::size_type insert_position = findInsertionPosition(values, index_list, current_value, right_bound);
	
			index_list.insert(index_list.begin() + insert_position, pending_block_end);
			DEBUG_MSG(4, "inserted at index_list[" << insert_position << "]");
		}
	}
	
	void PmergeMe::mergeInsertAtLevel(t_vector & values, t_vector::size_type block_size)
	{
		t_vector::size_type block_count = values.size() / block_size;
		t_vector::size_type pending_block_count = (block_count + 1) / 2;
	
		DEBUG_MSG(1, "block size = " << block_size << " | blocks = " << block_count << " | insertions = " << pending_block_count);
		
		if (pending_block_count <= 1)
		{
			DEBUG_MSG(2, DIM << "nothing changed" << RESET);
			return ;
		}
	
		t_index_list index_list = buildMainChainIndexList(block_count, block_size);
		DEBUG_MSG_CONTAINER(1, "indexes before: ", index_list);
	
		insertByJacobsthalOrder(values, index_list, block_size, pending_block_count);
		DEBUG_MSG_CONTAINER(1, "indexes after: ", index_list);
	
		values = reorderByIndexList(values, index_list, block_size, block_count);
	
		DEBUG_MSG_CONTAINER(1, "values after: ", values);
	}

// END_SECTION sort helpers

// SECTION sort

	// -SECTION vector

		void PmergeMe::sort(t_vector & values)
		{
			DEBUG_HEADER("VECTOR");
			t_vector::size_type	block_size = 1;

			// Phase 1: recursively sort pairs at increasing block sizes
			DEBUG_MSG(0, BOLD << "PAIR SORT" << RESET);
			while (block_size < (values.size() / 2))
			{
				sortPairs(values, block_size);
				block_size *= 2;
			}

			// Phase 2: merge-insert pending elements at decreasing block sizes
			DEBUG_MSG(0, BOLD << "INSERT" << RESET);
			while (block_size > 1)
			{
				block_size /= 2;
				mergeInsertAtLevel(values, block_size);
			}
		}

	// -END_SECTION vector

	void PmergeMe::sort(void)
	{
		setVectorTimer(std::clock());
		sort(_vector_container);
		setVectorTimer(std::clock() - getVectorTimer());
	
		// sort other container
	
		_sorted = true;
	}

// END_SECTION sort
