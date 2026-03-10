#include "RPN.hpp"
//#include <iostream> if output stream operator will be used

/**
 * @brief default constructor
*/
RPN::RPN(void)
{
	DEBUG_MSG("RPN default constructor called");
}

/**
 * @brief parameterized constructor
*/
/*
RPN::RPN(<all parameters of class>)
{
	DEBUG_MSG("RPN parameterized constructor called");
}
*/

/**
 * @brief copy constructor
 *
 * @param other object to copy
*/
RPN::RPN(const RPN &other)
{
	DEBUG_MSG("RPN copy constructor called");
}

/**
 * @brief destructor
*/
RPN::~RPN(void)
{
	DEBUG_MSG("RPN destructor called");
}

/**
 * @brief assignment operator
 *
 * @param other object to assign from
 *
 * @return reference to 'this' object
*/
RPN	&RPN::operator=(const RPN &other)
{
	DEBUG_MSG("RPN assignment operator called");
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
std::ostream	&operator<<(std::ostream &os, const RPN &c)
{
	os << "some info about RPN";
	return (os);
}
*/
