#ifndef PMERGEME_HPP
# define PMERGEME_HPP

// SECTION includes

	#include <ctime>	//std::clock_t
	#include <deque>	//std::deque
	#include <ostream>	//std::ostream
	#include <sstream>	//std::ostringstream
	#include <string>	//std::string
	#include <vector>	//std::vector

// END_SECTION includes

// SECTION class

	class PmergeMe
	{
		// SECTION variables

			public: 
				typedef std::vector<int>	t_vector;
				typedef std::deque<int>		t_deque;
			private:
				typedef std::vector<t_vector::size_type>	t_vector_index_list;
				typedef std::deque<t_deque::size_type>		t_deque_index_list;

				t_vector	_unsorted_vector;

				t_vector		_vector_container;
				unsigned int	_vector_comparison_count;
				std::clock_t	_vector_timer;

				t_deque			_deque_container;
				unsigned int	_deque_comparison_count;
				std::clock_t	_deque_timer;

		// END_SECTION variables

		// SECTION constructors

			private:
				PmergeMe(void);
				PmergeMe(const PmergeMe &other);
				PmergeMe	&operator=(const PmergeMe &other);
			public:
				~PmergeMe(void);
				PmergeMe(const std::string & value_sequence);
				PmergeMe(int argc, char ** argv);

		// END_SECTION constructors

		// SECTION accessors

			public:
				const t_vector	getUnsortedVector(void) const;

				const t_vector	getVectorContainer(void) const;
				const t_deque	getDequeContainer(void) const;

				unsigned int	getVectorComparisonCount(void) const;
				unsigned int	getDequeComparisonCount(void) const;

				std::clock_t	getVectorTimer(void) const;
				std::clock_t	getDequeTimer(void) const;

				void	setVectorTimer(std::clock_t time);
				void	setDequeTimer(std::clock_t time);

		// END_SECTION accessors
		
		// SECTION methods

			private:

			// SECTION helpers

				long long	compare(int value1, int value2, unsigned int & comparison_counter, int debug_msg_indent_lvl = 0);

				// SECTION vector

					void	swapBlockPair_vector(t_vector & values, t_vector::size_type pair_start_index, t_vector::size_type block_size);
					void	sortPairs_vector(t_vector & values, t_vector::size_type block_size);
			
					t_vector::size_type	getBlockEndIndex_vector(t_vector::size_type block_number, t_vector::size_type block_size) const;
					t_vector::size_type	getBlockStartIndex_vector(t_vector::size_type block_end_index, t_vector::size_type block_size) const;

					t_vector::size_type	findPartnerInMainChain_vector(const t_vector_index_list & index_list, t_vector::size_type partner_index, t_vector::size_type value_count) const;
					t_vector::size_type	findInsertionPosition_vector(const t_vector & values, const t_vector_index_list & index_list, int current_value, t_vector::size_type right_bound);

					t_vector_index_list	buildMainChainIndexList_vector(t_vector::size_type block_count, t_vector::size_type block_size) const;

					t_vector	reorderByIndexList_vector(const t_vector & values, const t_vector_index_list & index_list, t_vector::size_type block_size, t_vector::size_type block_count) const;

					void	insertGroupRange_vector(const t_vector & values, t_vector_index_list & index_list, t_vector::size_type block_size, t_vector::size_type group_lower_bound, t_vector::size_type group_upper_bound);
					void	insertByJacobsthalOrder_vector(const t_vector & values, t_vector_index_list & index_list, t_vector::size_type block_size, t_vector::size_type pending_block_count);
					void	mergeInsertAtLevel_vector(t_vector & values, t_vector::size_type block_size);

				// END_SECTION vector

				// SECTION deque

					void	swapBlockPair_deque(t_deque & values, t_deque::size_type pair_start_index, t_deque::size_type block_size);
					void	sortPairs_deque(t_deque & values, t_deque::size_type block_size);
			
					t_deque::size_type	getBlockEndIndex_deque(t_deque::size_type block_number, t_deque::size_type block_size) const;
					t_deque::size_type	getBlockStartIndex_deque(t_deque::size_type block_end_index, t_deque::size_type block_size) const;

					t_deque::size_type	findPartnerInMainChain_deque(const t_deque_index_list & index_list, t_deque::size_type partner_index, t_deque::size_type value_count) const;
					t_deque::size_type	findInsertionPosition_deque(const t_deque & values, const t_deque_index_list & index_list, int current_value, t_deque::size_type right_bound);

					t_deque_index_list	buildMainChainIndexList_deque(t_deque::size_type block_count, t_deque::size_type block_size) const;

					t_deque	reorderByIndexList_deque(const t_deque & values, const t_deque_index_list & index_list, t_deque::size_type block_size, t_deque::size_type block_count) const;

					void	insertGroupRange_deque(const t_deque & values, t_deque_index_list & index_list, t_deque::size_type block_size, t_deque::size_type group_lower_bound, t_deque::size_type group_upper_bound);
					void	insertByJacobsthalOrder_deque(const t_deque & values, t_deque_index_list & index_list, t_deque::size_type block_size, t_deque::size_type pending_block_count);
					void	mergeInsertAtLevel_deque(t_deque & values, t_deque::size_type block_size);

				// END_SECTION deque

			// END_SECTION helpers
			
				void	sortVector(t_vector & values);
				void	sortDeque(t_deque & values);
			public:
				void	sort(void);

		// END_SECTION methods
	};
	
	std::ostream	&operator<<(std::ostream &os, const PmergeMe &c);

// END_SECTION class

// SECTION helper functions

	template <typename Container>
	std::string	containerToString(const Container &container, std::size_t max_elements = 6)
	{
		typename Container::const_iterator	it = container.begin();
		std::ostringstream					oss;
		std::size_t							count = 0;
	
	#ifdef DEBUG
		max_elements = 0;
	#endif /* DEBUG */
	
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

// END_SECTION helper functions

// SECTION macros

	#define MICROSECONDS_PER_SECOND 1000000
	#define MICROSECONDS_PER_MILLI 1000

// END_SECTION macros

// SECTION debug

	# define RESET	"\033[0m"
	# define YELLOW	"\033[33m"
	# define CYAN	"\033[36m"
	# define BOLD	"\033[1m"
	# define DIM	"\033[2m"
	
	# ifdef DEBUG
	#  include <iostream>
	
	inline std::string indent(int level)
	{
		return (std::string(level * 2, ' '));
	}
	
	#  define DEBUG_MSG_LABEL_COLOR(level, color, label, msg) std::cerr << indent(level) << color << label << RESET << msg << std::endl
	#  define DEBUG_MSG_LABEL(level, label, msg) DEBUG_MSG_LABEL_COLOR(level, YELLOW, label, msg)
	#  define DEBUG_MSG(level, msg) DEBUG_MSG_LABEL(level, "> ", msg)
	
	#  define DEBUG_HEADER(description) std::cerr << std::endl << BOLD << CYAN << "══════ " << description << " ══════" << RESET << std::endl
	
	#  define DEBUG_MSG_CONTAINER(level, msg, container) DEBUG_MSG(level, DIM << msg << containerToString(container, 0) << RESET)
	# else
	#  define DEBUG_MSG_LABEL_COLOR(level, color, label, msg)
	#  define DEBUG_MSG_LABEL(level, label, msg)
	#  define DEBUG_MSG(level, msg)
	#  define DEBUG_HEADER(description)
	#  define DEBUG_MSG_CONTAINER(level, msg, container)
	# endif

// END_SECTION debug

#endif /* PMERGEME_HPP */
