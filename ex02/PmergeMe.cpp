#include "PmergeMe.hpp"
#include <cctype>		//std::isspace
#include <cerrno>		//errno, ERANGE
#include <cstdlib>		//std::strtol
#include <iostream>		//std::cerr/cout/endl
#include <limits>		//std::numeric_limits
#include <stdexcept>	//std::runtime_error

/**
 * @brief default constructor
*/
PmergeMe::PmergeMe(void)
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

/**
 * @brief parameterized constructor
 * 
 * Parses the given string into both containers (vector, deque).
 * 
 * @throws ...
 *   - @p value_string is empty/just whitespaces
 *   - @p value_string has non numeric characters
 *   - integer overflow occurs
 */
PmergeMe::PmergeMe(const std::string & value_string)
{
	DEBUG_MSG("PmergeMe parameterized constructor called");

	const char * iter = value_string.c_str();

	while (*iter != '\0')
	{
		char * end = NULL;
		errno = 0;

		long l_value = std::strtol(iter, &end, 10);

		if (errno == ERANGE
			|| l_value < std::numeric_limits<int>::min()
			|| l_value > std::numeric_limits<int>::max())
			throw std::runtime_error("number out of range");
		else if (iter == end)
			break ;

		int value = static_cast<int>(l_value);
		
		if (value < 0)
			throw std::runtime_error("negative number in sequence");
	
		DEBUG_MSG("adding to containers: " << value);
		_vector_container.push_back(value);
		_deque_container.push_back(value);

		iter = end;
	}

	while (std::isspace(static_cast<unsigned char>(*iter)))
		++iter;

	if (*iter != '\0')
		throw std::runtime_error("non-numeric character encountered");
}

const std::vector<int> PmergeMe::getVectorContainer(void) const
{
	return (_vector_container);
}

const std::deque<int> PmergeMe::getDequeContainer(void) const
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
