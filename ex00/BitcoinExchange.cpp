/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:19 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/20 14:18:26 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

/**
 * @brief default constructor
*/
BitcoinExchange::BitcoinExchange(void) :
	db_()
{
	DEBUG_MSG("BitcoinExchange default constructed.");
}

/**
 * @brief copy constructor
*/
BitcoinExchange::BitcoinExchange(const BitcoinExchange & other) :
	db_(other.db_)
{
	DEBUG_MSG("BitcoinExchange copy constructed.");
}

/**
 * @brief deconstructor
*/
BitcoinExchange::~BitcoinExchange(void)
{
	DEBUG_MSG("BitcoinExchange deconstructed.");
}

/**
 * @brief copy assignment operator
 */
BitcoinExchange & BitcoinExchange::operator=(const BitcoinExchange & other)
{
	DEBUG_MSG("BitcoinExchange copy assigned.");

	if (this != &other)
	{
		db_ = other.db_;
	}

	return (*this);
}

/**
 * @brief Splits a line into two parts based on a separator string.
 * 
 * @param line The string to be split. Must not be empty.
 * @param separator The delimiter string used to split the line. Must not be empty.
 * 
 * @return A pair of strings where:
 *         - first: substring from the beginning to the separator (exclusive)
 *         - second: substring from after the separator to the end
 * 
 * @throws BitcoinExchange::InvalidLineException if:
 *         - line is empty
 *         - separator is empty
 *         - separator is not found in line
 * 
 * @note The separator string is excluded from both parts of the result.
 *       If separator appears multiple times, only the first occurrence is used.
 */
std::pair<std::string, std::string> BitcoinExchange::splitLine(const std::string & line, const std::string & separator)
{
	std::pair<std::string, std::string> line_pair;

	if (line.empty() || separator.empty())
		throw BitcoinExchange::InvalidLineException("splitLine(): got empty string.");

	std::string::size_type separator_pos = line.find(separator);

	if (separator_pos == std::string::npos)
		throw BitcoinExchange::InvalidLineException("splitLine(): separator not found in 'line'.");

	line_pair.first = line.substr(0, separator_pos);
	line_pair.second = line.substr((separator_pos + separator.length()));

	return (line_pair);
}

/**
 * @brief checks if string is in correct format
 *
 * Parses a string, expecting the date format <YYYY-MM-DD>
 * (e.g. "20026-02-19"). If the format is wrong it will
 * throw an exception.
 *
 * @param date std::string, the string expected to be a date
 *
 * @return void
 *
 * @throw ?
 */
BitcoinExchange::s_date BitcoinExchange::parseDateString(const std::string & date_str)
{
	s_date date;
	//TODO
	(void)date_str;
	date.year = 1;
	return (date);
}

/**
 * @brief evaluates a date string
 *
 * First parses the string to be a valid date string.
 * Then it checks if the date is valid. (e.g. 2022-00-12 -> invalid)
 *
 * @param date std::string, string representing a date <YYYY-MM-DD>
 *
 * @return void
 *
 * @throw ?
 *
 * @see BitcoinExchange::parseDate()
 */
void BitcoinExchange::validateDate(const s_date & date)
{
	//TODO
	(void)date;
}

double BitcoinExchange::parseValueString(const std::string & value_str)
{
	//TODO
	(void)value_str;
	return (0);
}
double BitcoinExchange::parseValueString(const std::string & value_str, double min_value, double max_value)
{
	//TODO
	(void)value_str;
	(void)min_value;
	(void)max_value;
	return (0);
}

void BitcoinExchange::loadDatabase(const std::string & db_path)
{
	//TODO
	(void)db_path;
}

/**
 * @brief finds the rate for the given data
 *
 * Validates date string. Then it looks into the internal exchange_rates_ map
 * and searches for the latest bitcoin exchange rate in the database.
 * If the given date does not exist in the database, it will return the rate
 * of a older date nearest to the given date.
 *
 * @param date std::string, date to search the exchange-rate for, in <YYYY-MM-DD> format.
 *
 * @return double, the bitcoin exchange rate of that date
 *
 * @throw std::exception?, for incorrect date formatting
 *
 * @note if 'db_' is empty, throw/print an error
 */
double BitcoinExchange::getRate(const std::string & date) const
{
	//TODO
	(void)date;
	return (0);
}

/**
 * @brief get the money amount of the bitcoins
 *
 * Calls the 'getRate()' function to get the rate for that date.
 * Then it multiplies the 'bitcoin_amount' with that bitcoin rate.
 * That is the monetary value of that amount of bitcoins on that date.
 *
 * @param date std::string, date of the exchange rate
 * @param bitcoin_amount double, amount of bitcoins to exchange
 *
 * @return double, the monetary value of the bitcoins
 *
 * @note if 'db_' is empty, throw/print an error
 */
double BitcoinExchange::exchange(const std::string & date, double bitcoin_amount) const
{
	//TODO
	(void)date;
	(void)bitcoin_amount;
	return (0);
}

/**
 * @brief ...
 */
void BitcoinExchange::exchangeByFile(const std::string & file_path, const std::string & separator)
{
	//TODO
	(void)file_path;
	(void)separator;
}

// -------------------- EXCEPTIONS -------------------- //

BitcoinExchange::InvalidLineException::InvalidLineException(const std::string & msg) : std::runtime_error(msg) {}
BitcoinExchange::InvalidDateException::InvalidDateException(const std::string & msg) : std::runtime_error(msg) {}
BitcoinExchange::InvalidValueException::InvalidValueException(const std::string & msg) : std::runtime_error(msg) {}
BitcoinExchange::InvalidFileException::InvalidFileException(const std::string & msg) : std::runtime_error(msg) {}
