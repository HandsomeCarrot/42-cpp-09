#include "PmergeMe.hpp"
//#include <iostream> if output stream operator will be used

/**
 * @brief default constructor
*/
PmergeMe::PmergeMe(void)
{
	DEBUG_MSG("PmergeMe default constructor called");
}

/**
 * @brief parameterized constructor
*/
/*
PmergeMe::PmergeMe(<all parameters of class>)
{
	DEBUG_MSG("PmergeMe parameterized constructor called");
}
*/

/**
 * @brief copy constructor
 * 
 * @param other object to copy
*/
PmergeMe::PmergeMe(const PmergeMe &other)
{
	DEBUG_MSG("PmergeMe copy constructor called");
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

	}
	return (*this);
}

/** 
 * @brief output stream operator
 * 
 * @param os reference to the outputstream
 * @param class reference to the class object
 * 
 * @return reference to the output stream
*/
/*
std::ostream	&operator<<(std::ostream &os, const PmergeMe &c)
{
	os << "some info about PmergeMe";
	return (os);
}
*/
