// SECTION includes

	#include "PmergeMe.hpp"
	#include <climits>		//ULONG_MAX
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

	/**
	 * @brief parameterized constructor from command-line arguments
	 * 
	 * Parses an array of C-style strings representing non-negative integers into
	 * all 3 internal containers while preserving the input order.
	 *
	 * The constructor leaves the sorted-status flag set to false.
	 *
	 * @param argc number of arguments in argv array (must be at least 2)
	 * @param argv array of C-style strings, each representing a non-negative integer
	 * 
	 * @throws std::runtime_error
	 *   - if @p argc is less than 2
	 *   - if any @p argv[i] contains non-numeric characters
	 *   - if any parsed value is outside the `int` range
	 *   - if any parsed value is negative
	 */
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

	/**
	 * @brief Returns a copy of the unsorted input sequence.
	 * 
	 * The returned container holds the original values in the order they were
	 * provided to the constructor, before any sorting operation.
	 * 
	 * @return a const `t_vector` (std::vector<int>) containing the unsorted values
	 */
	const PmergeMe::t_vector PmergeMe::getUnsortedVector(void) const
	{
		return (_unsorted_vector);
	}
	
	/**
	 * @brief Returns a copy of the sorted vector container.
	 * 
	 * After `sort()` has been called, this container holds the values sorted in
	 * ascending order. Before sorting, it holds the unsorted input sequence.
	 * 
	 * @return a const `t_vector` (std::vector<int>) containing the sorted values
	 */
	const PmergeMe::t_vector PmergeMe::getVectorContainer(void) const
	{
		return (_vector_container);
	}
	
	/**
	 * @brief Returns a copy of the sorted deque container.
	 * 
	 * After `sort()` has been called, this container holds the values sorted in
	 * ascending order. Before sorting, it holds the unsorted input sequence.
	 * 
	 * @return a const `t_deque` (std::deque<int>) containing the sorted values
	 */
	const PmergeMe::t_deque PmergeMe::getDequeContainer(void) const
	{
		return (_deque_container);
	}
	
	/**
	 * @brief Returns the number of comparisons performed during vector sorting.
	 * 
	 * The counter is incremented each time two elements are compared by the
	 * internal `compare` function. It is reset to zero when the object is
	 * constructed and updated during the `sort()` operation.
	 * 
	 * @return the total number of comparisons made while sorting the vector
	 */
	unsigned int PmergeMe::getVectorComparisonCount(void) const
	{
		return (_vector_comparison_count);
	}
	
	/**
	 * @brief Returns the number of comparisons performed during deque sorting.
	 * 
	 * The counter is incremented each time two elements are compared by the
	 * internal `compare` function. It is reset to zero when the object is
	 * constructed and updated during the `sort()` operation.
	 * 
	 * @return the total number of comparisons made while sorting the deque
	 */
	unsigned int PmergeMe::getDequeComparisonCount(void) const
	{
		return (_deque_comparison_count);
	}
	
	/**
	 * @brief Returns the CPU time (in clock ticks) taken to sort the vector.
	 * 
	 * The timer is set to zero at construction. After `sort()` has been called,
	 * it holds the elapsed CPU clock ticks measured during the vector sorting
	 * phase. The value can be converted to a human-readable string via the
	 * stream operator.
	 * 
	 * @return the elapsed CPU clock ticks for vector sorting
	 */
	std::clock_t PmergeMe::getVectorTimer(void) const
	{
		return (_vector_timer);
	}
	
	/**
	 * @brief Returns the CPU time (in clock ticks) taken to sort the deque.
	 * 
	 * The timer is set to zero at construction. After `sort()` has been called,
	 * it holds the elapsed CPU clock ticks measured during the deque sorting
	 * phase. The value can be converted to a human-readable string via the
	 * stream operator.
	 * 
	 * @return the elapsed CPU clock ticks for deque sorting
	 */
	std::clock_t PmergeMe::getDequeTimer(void) const
	{
		return (_deque_timer);
	}
	
	/**
	 * @brief Sets the vector timer to a specific clock value.
	 * 
	 * This function is used internally by the sorting algorithm to store the
	 * elapsed CPU time after sorting the vector. It is not intended for external
	 * use.
	 * 
	 * @param time the clock ticks value to assign
	 */
	void PmergeMe::setVectorTimer(std::clock_t time)
	{
		_vector_timer = time;
	}
	
	/**
	 * @brief Sets the deque timer to a specific clock value.
	 * 
	 * This function is used internally by the sorting algorithm to store the
	 * elapsed CPU time after sorting the deque. It is not intended for external
	 * use.
	 * 
	 * @param time the clock ticks value to assign
	 */
	void PmergeMe::setDequeTimer(std::clock_t time)
	{
		_deque_timer = time;
	}

// END_SECTION accessors

// SECTION stream operator

	namespace
	{
		#ifdef DEBUG

			/**
			 * @brief Computes an upper bound for the number of comparisons in the Ford‑Johnson algorithm.
			 *
			 * The bound is derived from the theoretical worst‑case complexity of the
			 * merge‑insertion sort. It is used only for debugging purposes when the
			 * `DEBUG` macro is defined.
			 *
			 * @param element_count number of elements to be sorted
			 * @return an approximate upper bound on the number of comparisons required
			 */
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

			/**
			 * @brief Checks whether a vector is sorted in non‑decreasing order.
			 *
			 * Iterates over consecutive pairs of elements and verifies that each
			 * element is less than or equal to its successor.
			 *
			 * @param container the vector to test
			 * @return `true` if the vector is sorted, `false` otherwise
			 */
			bool isVectorSorted(const PmergeMe::t_vector container)
			{
				for (PmergeMe::t_vector::const_iterator i = container.begin(); i + 1 != container.end(); ++i)
				{
					if (*i > *(i + 1))
						return (false);
				}
				return (true);
			}

			/**
			 * @brief Checks whether a deque is sorted in non‑decreasing order.
			 *
			 * Iterates over consecutive pairs of elements and verifies that each
			 * element is less than or equal to its successor.
			 *
			 * @param container the deque to test
			 * @return `true` if the deque is sorted, `false` otherwise
			 */
			bool isDequeSorted(const PmergeMe::t_deque container)
			{
				for (PmergeMe::t_deque::const_iterator i = container.begin(); i + 1 != container.end(); ++i)
				{
					if (*i > *(i + 1))
						return (false);
				}
				return (true);
			}

		#endif /* DEBUG */
	
		/**
		 * @brief Converts a clock‑tick value into a human‑readable time string.
		 *
		 * The string format is "seconds s milliseconds ms microseconds us".
		 * Negative input values trigger a warning and produce "0". Overflow during
		 * the conversion also results in a warning and "0".
		 *
		 * @param ticks CPU clock ticks (as returned by `std::clock()`)
		 * @return a formatted time string, or "0" on error
		 */
		std::string clockToString(std::clock_t ticks)
		{
			if (ticks < 0)
			{
				std::cerr << "Warning: negative time value given: canceled operation!" << std::endl;
				return ("0");
			}

			unsigned long seconds = static_cast<unsigned long>(ticks) / CLOCKS_PER_SEC;

			unsigned long leftover_ticks = static_cast<unsigned long>(ticks) % CLOCKS_PER_SEC;

			// check if multiplication would overflow
			if (MICROSECONDS_PER_SECOND != 0 && leftover_ticks > (ULONG_MAX / MICROSECONDS_PER_SECOND))
			{
				std::cerr << "Warning: calculation overflow in time conversion: canceled operation!" << std::endl;
				return ("0");
			}

			unsigned long leftover_microseconds = (leftover_ticks * MICROSECONDS_PER_SECOND) / CLOCKS_PER_SEC;

			unsigned long milliseconds = leftover_microseconds / MICROSECONDS_PER_MILLI;
			unsigned long microseconds = leftover_microseconds % MICROSECONDS_PER_MILLI;

			std::ostringstream time_string;
			time_string << seconds << " s "
						<< milliseconds << " ms "
						<< microseconds << " us";

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

			os << "sorted: "
				<< std::boolalpha
				<< "vector = " << isVectorSorted(c.getVectorContainer())
				<< ", deque = " << isDequeSorted(c.getDequeContainer())
				<< std::endl;
		
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

	// SECTION universal

		/**
		 * @brief Compares two values while tracking the number of comparisons made.
		 *
		 * Increments @p comparison_counter before returning the arithmetic difference
		 * between @p value1 and @p value2. If the counter has already reached its
		 * maximum representable value, a warning is printed before the increment,
		 * meaning the next increment wraps according to `unsigned int` semantics.
		 * The @p debug_msg_indent_lvl parameter controls the indentation level of
		 * debug output when the DEBUG macro is defined.
		 *
		 * @param value1 first value to compare
		 * @param value2 second value to compare
		 * @param comparison_counter reference to the comparison counter to update
		 * @param debug_msg_indent_lvl indentation level for debug messages (defaults to 0)
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

	// END_SECTION universal

	// SECTION vector

		/**
		 * @brief Swaps two adjacent blocks of equal size within a vector.
		 *
		 * Given a starting index @p pair_start_index and a block size @p block_size,
		 * this function exchanges the block that starts at @p pair_start_index with
		 * the block that starts at @p pair_start_index + @p block_size. Both blocks
		 * have exactly @p block_size elements.
		 *
		 * @param values the vector whose blocks are to be swapped
		 * @param pair_start_index index of the first element of the left block
		 * @param block_size number of elements in each block
		 */
		void PmergeMe::swapBlockPair_vector(t_vector & values, t_vector::size_type pair_start_index, t_vector::size_type block_size)
		{
			t_vector::iterator left_block_begin = values.begin() + pair_start_index;
			t_vector::iterator right_block_begin = values.begin() + pair_start_index + block_size;
		
			std::swap_ranges(left_block_begin, right_block_begin, right_block_begin);
		}

		/**
		 * @brief Sorts adjacent pairs of blocks within a vector.
		 *
		 * Iterates over the vector in steps of `2 * block_size`. For each pair of
		 * consecutive blocks, compares the last element of the left block with the
		 * last element of the right block. If the left block's last element is
		 * greater, the two blocks are swapped so that the larger block ends up on
		 * the right side. This establishes an ordering between blocks that is used
		 * in later merge-insert steps.
		 *
		 * @param values the vector whose block pairs are to be sorted
		 * @param block_size number of elements in each block
		 */
		void PmergeMe::sortPairs_vector(t_vector & values, t_vector::size_type block_size)
		{
			DEBUG_MSG(1, "block size = " << block_size);
			for (t_vector::size_type pair_start_index = 0; pair_start_index + (2 * block_size) <= values.size(); pair_start_index += (2 * block_size))
			{
				t_vector::size_type left_block_last_index = pair_start_index + block_size - 1;
				t_vector::size_type right_block_last_index = left_block_last_index + block_size;
		
				if (compare(values[left_block_last_index], values[right_block_last_index], _vector_comparison_count, 2) > 0)
					swapBlockPair_vector(values, pair_start_index, block_size);
			}
			DEBUG_MSG_CONTAINER(1, "values after: ", values);
		}
		
		/**
		 * @brief Computes the index of the last element of a block.
		 *
		 * Blocks are numbered starting from 1. Each block contains exactly
		 * @p block_size consecutive elements. The last element of block number
		 * @p block_number is at index `(block_number * block_size) - 1`.
		 *
		 * @param block_number 1‑based block number
		 * @param block_size number of elements per block
		 * @return zero‑based index of the block's last element
		 */
		PmergeMe::t_vector::size_type PmergeMe::getBlockEndIndex_vector(t_vector::size_type block_number, t_vector::size_type block_size) const
		{
			return ((block_number * block_size) - 1);
		}
		
		/**
		 * @brief Computes the index of the first element of a block given its last element.
		 *
		 * Given the index of the last element of a block and the block size,
		 * returns the index of the first element of that block.
		 *
		 * @param block_end_index zero‑based index of the block's last element
		 * @param block_size number of elements per block
		 * @return zero‑based index of the block's first element
		 */
		PmergeMe::t_vector::size_type PmergeMe::getBlockStartIndex_vector(t_vector::size_type block_end_index, t_vector::size_type block_size) const
		{
			return (block_end_index - block_size + 1);
		}
		
		/**
		 * @brief Locates the position of a partner block's index within the main‑chain index list.
		 *
		 * The main‑chain index list stores the end indices of blocks that have already
		 * been inserted into the sorted main chain. This function searches for the
		 * index @p partner_index (which is the end index of a pending block's partner)
		 * inside @p index_list. If the partner index is not present (or if the partner
		 * index lies outside the valid range of values), the function returns the size
		 * of the index list, indicating that the partner is not in the main chain.
		 *
		 * @param index_list list of end indices of blocks already in the main chain
		 * @param partner_index end index of the partner block to locate
		 * @param value_count total number of elements in the original container
		 * @return position of @p partner_index in @p index_list, or `index_list.size()`
		 *         if the partner is not found
		 */
		PmergeMe::t_vector::size_type PmergeMe::findPartnerInMainChain_vector(
			const t_vector_index_list & index_list,
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
		
		/**
		 * @brief Performs a binary search to find the insertion position of a value within the main chain.
		 *
		 * The main chain is represented indirectly by an index list that holds the
		 * end indices of its blocks. This function searches for the position where
		 * @p current_value should be inserted so that the main chain remains sorted.
		 * The search is limited to the range `[0, right_bound)` within the index list.
		 *
		 * @param values the original vector of all elements
		 * @param index_list list of end indices of blocks already in the main chain
		 * @param current_value the value whose insertion position is to be determined
		 * @param right_bound exclusive upper bound for the search within the index list
		 * @return the index (within @p index_list) where @p current_value should be inserted
		 */
		PmergeMe::t_vector::size_type PmergeMe::findInsertionPosition_vector(
			const t_vector & values,
			const t_vector_index_list & index_list,
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
		
		/**
		 * @brief Constructs the initial main‑chain index list for a given block size.
		 *
		 * The initial main chain consists of the last element of every even‑numbered
		 * block (blocks 2, 4, 6, …) plus the last element of the first block (block 1).
		 * The resulting list contains the end indices of those blocks that are already
		 * considered “sorted” after the pair‑sorting phase.
		 *
		 * @param block_count total number of blocks (each of size @p block_size)
		 * @param block_size number of elements per block
		 * @return a vector containing the end indices of the blocks that form the initial main chain
		 */
		PmergeMe::t_vector_index_list PmergeMe::buildMainChainIndexList_vector(
			t_vector::size_type block_count,
			t_vector::size_type block_size) const
		{
			t_vector_index_list	index_list;
		
			index_list.reserve(block_count);
		
			if (block_count > 0)
				index_list.push_back(getBlockEndIndex_vector(1, block_size));
		
			for (t_vector::size_type block_number = 2; block_number <= block_count; block_number += 2)
				index_list.push_back(getBlockEndIndex_vector(block_number, block_size));
		
			return (index_list);
		}
	
		/**
		 * @brief Inserts pending blocks into the main chain following the Jacobsthal sequence.
		 *
		 * The Jacobsthal sequence determines the order in which groups of pending blocks
		 * are inserted. Each group consists of a contiguous range of block numbers.
		 * Starting with group 2‑3, then 4‑5, 7‑11, etc., the function inserts each group
		 * in reverse order (largest block number first) to maintain the desired
		 * merge‑insertion properties.
		 *
		 * @param values the original vector of all elements
		 * @param index_list the main‑chain index list (will be updated with new block indices)
		 * @param block_size number of elements per block
		 * @param pending_block_count number of pending blocks waiting to be inserted
		 */
		void PmergeMe::insertByJacobsthalOrder_vector(
			const t_vector & values,
			t_vector_index_list & index_list,
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
		
				DEBUG_MSG(2, "groups [" << capped_group_end << ", " << group_start << "]");
		
				insertGroupRange_vector(values, index_list, block_size, group_start, capped_group_end);
		
				group_start = group_end + 1;
				t_vector::size_type next_group_end = group_end + (2 * prev_group_end);
				prev_group_end = group_end;
				group_end = next_group_end;
			}
		}
		
		/**
		 * @brief Reconstructs a sorted vector by traversing blocks in the order given by the index list.
		 *
		 * The index list specifies the order in which blocks should appear in the final
		 * sorted sequence. This function iterates over the index list, extracts each
		 * block entirely (from its first to its last element), and appends the elements
		 * to a new vector. Any leftover elements that do not belong to a full block
		 * (i.e., elements beyond `block_count * block_size`) are appended at the end.
		 *
		 * @param values the original vector of all elements
		 * @param index_list list of end indices of blocks in the desired order
		 * @param block_size number of elements per block
		 * @param block_count total number of full blocks
		 * @return a new vector containing all elements of @p values sorted according to @p index_list
		 */
		PmergeMe::t_vector PmergeMe::reorderByIndexList_vector(
			const t_vector & values,
			const t_vector_index_list & index_list,
			t_vector::size_type block_size,
			t_vector::size_type block_count) const
		{
			t_vector	sorted_values;
		
			sorted_values.reserve(values.size());
		
			for (t_vector::size_type i = 0; i < index_list.size(); ++i)
			{
				t_vector::size_type block_end = index_list[i];
				t_vector::size_type block_start = getBlockStartIndex_vector(block_end, block_size);
		
				for (t_vector::size_type value_index = block_start; value_index <= block_end; ++value_index)
					sorted_values.push_back(values[value_index]);
			}
		
			for (t_vector::size_type i = block_count * block_size; i < values.size(); ++i)
				sorted_values.push_back(values[i]);
		
			return (sorted_values);
		}
		
		/**
		 * @brief Inserts a contiguous range of pending blocks into the main chain.
		 *
		 * The range is defined by @p group_lower_bound and @p group_upper_bound,
		 * inclusive. Blocks are inserted in reverse order (largest group number first)
		 * to preserve the merge‑insertion algorithm's invariants. For each block,
		 * the function locates its partner in the main chain, determines the correct
		 * insertion position via binary search, and inserts the block's end index
		 * into the index list.
		 *
		 * @param values the original vector of all elements
		 * @param index_list the main‑chain index list (will be updated)
		 * @param block_size number of elements per block
		 * @param group_lower_bound smallest group number in the range (inclusive)
		 * @param group_upper_bound largest group number in the range (inclusive)
		 */
		void PmergeMe::insertGroupRange_vector(
			const t_vector & values,
			t_vector_index_list & index_list,
			t_vector::size_type block_size,
			t_vector::size_type group_lower_bound,
			t_vector::size_type group_upper_bound)
		{
			for (t_vector::size_type current_group = group_upper_bound; current_group >= group_lower_bound; --current_group)
			{
				t_vector::size_type pending_block_number = (2 * current_group) - 1;
				t_vector::size_type pending_block_end = getBlockEndIndex_vector(pending_block_number, block_size);
		
				DEBUG_MSG(3, "insert group " << current_group << ": v[" << pending_block_end << "] = " << values[pending_block_end]);
		
				int	current_value = values[pending_block_end];
				t_vector::size_type partner_index = pending_block_end + block_size;
				t_vector::size_type right_bound = findPartnerInMainChain_vector(index_list, partner_index, values.size());
				t_vector::size_type insert_position = findInsertionPosition_vector(values, index_list, current_value, right_bound);
		
				index_list.insert(index_list.begin() + insert_position, pending_block_end);
				DEBUG_MSG(4, "inserted at index_list[" << insert_position << "]");
			}
		}
		
		/**
		 * @brief Performs one level of the merge‑insertion sort on a vector.
		 *
		 * Given a current block size, this function builds the initial main chain,
		 * inserts the pending blocks following the Jacobsthal sequence, and finally
		 * reorders the vector according to the updated index list. If there is at
		 * most one pending block, the vector is already ordered at this level and
		 * no work is performed.
		 *
		 * @param values the vector to be sorted at the current block size
		 * @param block_size number of elements per block at this level
		 */
		void PmergeMe::mergeInsertAtLevel_vector(t_vector & values, t_vector::size_type block_size)
		{
			t_vector::size_type block_count = values.size() / block_size;
			t_vector::size_type pending_block_count = (block_count + 1) / 2;
		
			DEBUG_MSG(1, "block size = " << block_size << " | blocks = " << block_count << " | insertions = " << pending_block_count);
			
			if (pending_block_count <= 1)
			{
				DEBUG_MSG(2, DIM << "nothing changed" << RESET);
				return ;
			}
		
			t_vector_index_list index_list = buildMainChainIndexList_vector(block_count, block_size);
			DEBUG_MSG_CONTAINER(1, "indexes before: ", index_list);
		
			insertByJacobsthalOrder_vector(values, index_list, block_size, pending_block_count);
			DEBUG_MSG_CONTAINER(1, "indexes after: ", index_list);
		
			values = reorderByIndexList_vector(values, index_list, block_size, block_count);
		
			DEBUG_MSG_CONTAINER(1, "values after: ", values);
		}

	// END_SECTION vector

	// SECTION deque
		
		/**
		 * @brief Swaps two adjacent blocks of equal size within a deque.
		 *
		 * Given a starting index @p pair_start_index and a block size @p block_size,
		 * this function exchanges the block that starts at @p pair_start_index with
		 * the block that starts at @p pair_start_index + @p block_size. Both blocks
		 * have exactly @p block_size elements.
		 *
		 * @param values the deque whose blocks are to be swapped
		 * @param pair_start_index index of the first element of the left block
		 * @param block_size number of elements in each block
		 */
		void PmergeMe::swapBlockPair_deque(t_deque & values, t_deque::size_type pair_start_index, t_deque::size_type block_size)
		{
			t_deque::iterator left_block_begin = values.begin() + pair_start_index;
			t_deque::iterator right_block_begin = values.begin() + pair_start_index + block_size;
		
			std::swap_ranges(left_block_begin, right_block_begin, right_block_begin);
		}

		/**
		 * @brief Sorts adjacent pairs of blocks within a deque.
		 *
		 * Iterates over the deque in steps of `2 * block_size`. For each pair of
		 * consecutive blocks, compares the last element of the left block with the
		 * last element of the right block. If the left block's last element is
		 * greater, the two blocks are swapped so that the larger block ends up on
		 * the right side. This establishes an ordering between blocks that is used
		 * in later merge‑insert steps.
		 *
		 * @param values the deque whose block pairs are to be sorted
		 * @param block_size number of elements in each block
		 */
		void PmergeMe::sortPairs_deque(t_deque & values, t_deque::size_type block_size)
		{
			DEBUG_MSG(1, "block size = " << block_size);
			for (t_deque::size_type pair_start_index = 0; pair_start_index + (2 * block_size) <= values.size(); pair_start_index += (2 * block_size))
			{
				t_deque::size_type left_block_last_index = pair_start_index + block_size - 1;
				t_deque::size_type right_block_last_index = left_block_last_index + block_size;

				if (compare(values[left_block_last_index], values[right_block_last_index], _deque_comparison_count, 2) > 0)
					swapBlockPair_deque(values, pair_start_index, block_size);
			}
			DEBUG_MSG_CONTAINER(1, "values after: ", values);
		}

		/**
		 * @brief Computes the index of the last element of a block.
		 *
		 * Blocks are numbered starting from 1. Each block contains exactly
		 * @p block_size consecutive elements. The last element of block number
		 * @p block_number is at index `(block_number * block_size) - 1`.
		 *
		 * @param block_number 1‑based block number
		 * @param block_size number of elements per block
		 * @return zero‑based index of the block's last element
		 */
		PmergeMe::t_deque::size_type PmergeMe::getBlockEndIndex_deque(t_deque::size_type block_number, t_deque::size_type block_size) const
		{
			return ((block_number * block_size) - 1);
		}

		/**
		 * @brief Computes the index of the first element of a block given its last element.
		 *
		 * Given the index of the last element of a block and the block size,
		 * returns the index of the first element of that block.
		 *
		 * @param block_end_index zero‑based index of the block's last element
		 * @param block_size number of elements per block
		 * @return zero‑based index of the block's first element
		 */
		PmergeMe::t_deque::size_type PmergeMe::getBlockStartIndex_deque(t_deque::size_type block_end_index, t_deque::size_type block_size) const
		{
			return (block_end_index - block_size + 1);
		}

		/**
		 * @brief Locates the position of a partner block's index within the main‑chain index list.
		 *
		 * The main‑chain index list stores the end indices of blocks that have already
		 * been inserted into the sorted main chain. This function searches for the
		 * index @p partner_index (which is the end index of a pending block's partner)
		 * inside @p index_list. If the partner index is not present (or if the partner
		 * index lies outside the valid range of values), the function returns the size
		 * of the index list, indicating that the partner is not in the main chain.
		 *
		 * @param index_list list of end indices of blocks already in the main chain
		 * @param partner_index end index of the partner block to locate
		 * @param value_count total number of elements in the original container
		 * @return position of @p partner_index in @p index_list, or `index_list.size()`
		 *         if the partner is not found
		 */
		PmergeMe::t_deque::size_type PmergeMe::findPartnerInMainChain_deque(
			const t_deque_index_list & index_list,
			t_deque::size_type partner_index,
			t_deque::size_type value_count) const
		{
			t_deque::size_type partner_position = 0;

			if (partner_index >= value_count)
				return (index_list.size());

			while (partner_position < index_list.size() && index_list[partner_position] != partner_index)
				++partner_position;

			return (partner_position);
		}
		
		/**
		 * @brief Performs a binary search to find the insertion position of a value within the main chain.
		 *
		 * The main chain is represented indirectly by an index list that holds the
		 * end indices of its blocks. This function searches for the position where
		 * @p current_value should be inserted so that the main chain remains sorted.
		 * The search is limited to the range `[0, right_bound)` within the index list.
		 *
		 * @param values the original deque of all elements
		 * @param index_list list of end indices of blocks already in the main chain
		 * @param current_value the value whose insertion position is to be determined
		 * @param right_bound exclusive upper bound for the search within the index list
		 * @return the index (within @p index_list) where @p current_value should be inserted
		 */
		PmergeMe::t_deque::size_type PmergeMe::findInsertionPosition_deque(
			const t_deque & values,
			const t_deque_index_list & index_list,
			int current_value,
			t_deque::size_type right_bound)
		{
			t_deque::size_type left = 0;
			t_deque::size_type right = right_bound;
		
			DEBUG_MSG(4, DIM << "binary search window [" << left << ", " << right << ")" << RESET);
		
			while (left < right)
			{
				t_deque::size_type mid = left + ((right - left) / 2);
		
				if (compare(current_value, values[index_list[mid]], _deque_comparison_count, 5) < 0)
					right = mid;
				else
					left = mid + 1;
			}
		
			return (left);
		}
		
		/**
		 * @brief Constructs the initial main‑chain index list for a given block size.
		 *
		 * The initial main chain consists of the last element of every even‑numbered
		 * block (blocks 2, 4, 6, …) plus the last element of the first block (block 1).
		 * The resulting list contains the end indices of those blocks that are already
		 * considered “sorted” after the pair‑sorting phase.
		 *
		 * @param block_count total number of blocks (each of size @p block_size)
		 * @param block_size number of elements per block
		 * @return a deque containing the end indices of the blocks that form the initial main chain
		 */
		PmergeMe::t_deque_index_list PmergeMe::buildMainChainIndexList_deque(
			t_deque::size_type block_count,
			t_deque::size_type block_size) const
		{
			t_deque_index_list	index_list;
		
			if (block_count > 0)
				index_list.push_back(getBlockEndIndex_deque(1, block_size));
		
			for (t_deque::size_type block_number = 2; block_number <= block_count; block_number += 2)
				index_list.push_back(getBlockEndIndex_deque(block_number, block_size));
		
			return (index_list);
		}
	
		/**
		 * @brief Inserts pending blocks into the main chain following the Jacobsthal sequence.
		 *
		 * The Jacobsthal sequence determines the order in which groups of pending blocks
		 * are inserted. Each group consists of a contiguous range of block numbers.
		 * Starting with group 2‑3, then 4‑6, 7‑12, etc., the function inserts each group
		 * in reverse order (largest block number first) to maintain the desired
		 * merge‑insertion properties.
		 *
		 * @param values the original deque of all elements
		 * @param index_list the main‑chain index list (will be updated with new block indices)
		 * @param block_size number of elements per block
		 * @param pending_block_count number of pending blocks waiting to be inserted
		 */
		void PmergeMe::insertByJacobsthalOrder_deque(
			const t_deque & values,
			t_deque_index_list & index_list,
			t_deque::size_type block_size,
			t_deque::size_type pending_block_count)
		{
			// group_end: the current jacobsthal-number (a number of the jacobsthal-sequence),
			// but also the index for a (number)group in the deque
			t_deque::size_type group_start = 2;
			t_deque::size_type group_end = 3;
			t_deque::size_type prev_group_end = 1;
		
			while (group_start <= pending_block_count)
			{
				// cap to last block
				t_deque::size_type capped_group_end = std::min(group_end, pending_block_count);
		
				DEBUG_MSG(2, "groups [" << capped_group_end << ", " << group_start << "]");
		
				insertGroupRange_deque(values, index_list, block_size, group_start, capped_group_end);
		
				group_start = group_end + 1;
				t_deque::size_type next_group_end = group_end + (2 * prev_group_end);
				prev_group_end = group_end;
				group_end = next_group_end;
			}
		}
		
		/**
		 * @brief Reconstructs a sorted deque by traversing blocks in the order given by the index list.
		 *
		 * The index list specifies the order in which blocks should appear in the final
		 * sorted sequence. This function iterates over the index list, extracts each
		 * block entirely (from its first to its last element), and appends the elements
		 * to a new deque. Any leftover elements that do not belong to a full block
		 * (i.e., elements beyond `block_count * block_size`) are appended at the end.
		 *
		 * @param values the original deque of all elements
		 * @param index_list list of end indices of blocks in the desired order
		 * @param block_size number of elements per block
		 * @param block_count total number of full blocks
		 * @return a new deque containing all elements of @p values sorted according to @p index_list
		 */
		PmergeMe::t_deque PmergeMe::reorderByIndexList_deque(
			const t_deque & values,
			const t_deque_index_list & index_list,
			t_deque::size_type block_size,
			t_deque::size_type block_count) const
		{
			t_deque	sorted_values;

			for (t_deque::size_type i = 0; i < index_list.size(); ++i)
			{
				t_deque::size_type block_end = index_list[i];
				t_deque::size_type block_start = getBlockStartIndex_deque(block_end, block_size);

				for (t_deque::size_type value_index = block_start; value_index <= block_end; ++value_index)
					sorted_values.push_back(values[value_index]);
			}

			for (t_deque::size_type i = block_count * block_size; i < values.size(); ++i)
				sorted_values.push_back(values[i]);

			return (sorted_values);
		}

		/**
		 * @brief Inserts a contiguous range of pending blocks into the main chain.
		 *
		 * The range is defined by @p group_lower_bound and @p group_upper_bound,
		 * inclusive. Blocks are inserted in reverse order (largest group number first)
		 * to preserve the merge‑insertion algorithm's invariants. For each block,
		 * the function locates its partner in the main chain, determines the correct
		 * insertion position via binary search, and inserts the block's end index
		 * into the index list.
		 *
		 * @param values the original deque of all elements
		 * @param index_list the main‑chain index list (will be updated)
		 * @param block_size number of elements per block
		 * @param group_lower_bound smallest group number in the range (inclusive)
		 * @param group_upper_bound largest group number in the range (inclusive)
		 */
		void PmergeMe::insertGroupRange_deque(
			const t_deque & values,
			t_deque_index_list & index_list,
			t_deque::size_type block_size,
			t_deque::size_type group_lower_bound,
			t_deque::size_type group_upper_bound)
		{
			for (t_deque::size_type current_group = group_upper_bound; current_group >= group_lower_bound; --current_group)
			{
				t_deque::size_type pending_block_number = (2 * current_group) - 1;
				t_deque::size_type pending_block_end = getBlockEndIndex_deque(pending_block_number, block_size);

				DEBUG_MSG(3, "insert group " << current_group << ": v[" << pending_block_end << "] = " << values[pending_block_end]);

				int	current_value = values[pending_block_end];
				t_deque::size_type partner_index = pending_block_end + block_size;
				t_deque::size_type right_bound = findPartnerInMainChain_deque(index_list, partner_index, values.size());
				t_deque::size_type insert_position = findInsertionPosition_deque(values, index_list, current_value, right_bound);

				index_list.insert(index_list.begin() + insert_position, pending_block_end);
				DEBUG_MSG(4, "inserted at index_list[" << insert_position << "]");
			}
		}
	
		/**
		 * @brief Performs one level of the merge‑insertion sort on a deque.
		 *
		 * Given a current block size, this function builds the initial main chain,
		 * inserts the pending blocks following the Jacobsthal sequence, and finally
		 * reorders the deque according to the updated index list. If there is at
		 * most one pending block, the deque is already ordered at this level and
		 * no work is performed.
		 *
		 * @param values the deque to be sorted at the current block size
		 * @param block_size number of elements per block at this level
		 */
		void PmergeMe::mergeInsertAtLevel_deque(t_deque & values, t_deque::size_type block_size)
		{
			t_deque::size_type block_count = values.size() / block_size;
			t_deque::size_type pending_block_count = (block_count + 1) / 2;

			DEBUG_MSG(1, "block size = " << block_size << " | blocks = " << block_count << " | insertions = " << pending_block_count);
		
			if (pending_block_count <= 1)
			{
				DEBUG_MSG(2, DIM << "nothing changed" << RESET);
				return ;
			}

			t_deque_index_list index_list = buildMainChainIndexList_deque(block_count, block_size);
			DEBUG_MSG_CONTAINER(1, "indexes before: ", index_list);

			insertByJacobsthalOrder_deque(values, index_list, block_size, pending_block_count);
			DEBUG_MSG_CONTAINER(1, "indexes after: ", index_list);

			values = reorderByIndexList_deque(values, index_list, block_size, block_count);

			DEBUG_MSG_CONTAINER(1, "values after: ", values);
		}

	// END_SECTION deque

// END_SECTION sort helpers

// SECTION sort

	// SECTION specialized sorts

		/**
		 * @brief Sorts a vector using the Ford‑Johnson merge‑insertion algorithm.
		 *
		 * The algorithm proceeds in two phases:
		 * 1. Pair‑sorting: repeatedly groups elements into blocks of increasing size,
		 *    sorts each pair of blocks by comparing their last elements, and swaps
		 *    blocks when necessary.
		 * 2. Merge‑insertion: processes blocks in decreasing size, inserting pending
		 *    blocks into the main chain according to the Jacobsthal sequence.
		 *
		 * The function modifies the input vector in‑place and updates the internal
		 * comparison counter for vector operations.
		 *
		 * @param values the vector to be sorted (will be modified)
		 */
		void PmergeMe::sortVector(t_vector & values)
		{
			DEBUG_HEADER("VECTOR");
			t_vector::size_type	block_size = 1;

			// Phase 1: recursively sort pairs at increasing block sizes
			DEBUG_MSG(0, BOLD << "PAIR SORT" << RESET);
			while (block_size <= (values.size() / 2))
			{
				sortPairs_vector(values, block_size);
				block_size *= 2;
			}

			// Phase 2: merge-insert pending elements at decreasing block sizes
			DEBUG_MSG(0, BOLD << "INSERT" << RESET);
			while (block_size > 1)
			{
				block_size /= 2;
				mergeInsertAtLevel_vector(values, block_size);
			}
		}

		/**
		 * @brief Sorts a deque using the Ford‑Johnson merge‑insertion algorithm.
		 *
		 * The algorithm proceeds in two phases:
		 * 1. Pair‑sorting: repeatedly groups elements into blocks of increasing size,
		 *    sorts each pair of blocks by comparing their last elements, and swaps
		 *    blocks when necessary.
		 * 2. Merge‑insertion: processes blocks in decreasing size, inserting pending
		 *    blocks into the main chain according to the Jakobsthal sequence.
		 *
		 * The function modifies the input deque in‑place and updates the internal
		 * comparison counter for deque operations.
		 *
		 * @param values the deque to be sorted (will be modified)
		 */
		void PmergeMe::sortDeque(t_deque & values)
		{
			DEBUG_HEADER("DEQUE");
			t_deque::size_type	block_size = 1;

			// Phase 1: recursively sort pairs at increasing block sizes
			DEBUG_MSG(0, BOLD << "PAIR SORT" << RESET);
			while (block_size <= (values.size() / 2))
			{
				sortPairs_deque(values, block_size);
				block_size *= 2;
			}

			// Phase 2: merge-insert pending elements at decreasing block sizes
			DEBUG_MSG(0, BOLD << "INSERT" << RESET);
			while (block_size > 1)
			{
				block_size /= 2;
				mergeInsertAtLevel_deque(values, block_size);
			}
		}

	// END_SECTION specialized sorts

	// SECTION time helpers

		namespace
		{
			/**
			 * @brief Retrieves the current CPU clock time.
			 *
			 * Wraps `std::clock()` and checks for the error value `(std::clock_t)(-1)`.
			 *
			 * @return the current processor time as a `std::clock_t`
			 * @throws std::runtime_error if `std::clock()` fails
			 */
			std::clock_t getCurrentClock(void)
			{
				std::clock_t time = std::clock();
				if (time == static_cast<std::clock_t>(-1))
					throw std::runtime_error("failed to get current time");
				return (time);
			}

			/**
			 * @brief Computes the CPU clock ticks elapsed since a given timestamp.
			 *
			 * The function ensures that the current clock is not earlier than the
			 * provided timestamp (which would indicate a broken or non‑monotonic timer).
			 *
			 * @param timestamp a past clock value obtained from `getCurrentClock()`
			 * @return the difference `current_clock - timestamp`
			 * @throws std::runtime_error if the current clock appears to be earlier than @p timestamp
			 */
			std::clock_t getElapsedClock(std::clock_t timestamp)
			{
				std::clock_t current_clock = getCurrentClock();
				if (current_clock < timestamp)
					throw std::runtime_error("timer is broken");
				return (current_clock - timestamp);
			}
		}

	// END_SECTION time helpers

	/**
	 * @brief Sorts both internal containers (vector and deque) and records the CPU time taken.
	 *
	 * The function performs the Ford‑Johnson merge‑insertion sort on the vector and
	 * the deque separately. For each container, it records the start time, runs the
	 * sorting algorithm, and stores the elapsed CPU clock ticks in the corresponding
	 * timer member. The comparison counters are updated by the internal sorting
	 * routines.
	 */
	void PmergeMe::sort(void)
	{
		// vector
		setVectorTimer(getCurrentClock());
		sortVector(_vector_container);
		setVectorTimer(getElapsedClock(getVectorTimer()));

		// deque
		setDequeTimer(getCurrentClock());
		sortDeque(_deque_container);
		setDequeTimer(getElapsedClock(getDequeTimer()));
	}

// END_SECTION sort
