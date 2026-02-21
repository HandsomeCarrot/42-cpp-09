/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:19 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/21 19:18:56 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <stdexcept>

// -------------------- HELPER FUNCTIONS -------------------- //

namespace {
	struct s_date
	{
		unsigned int	year;
		unsigned int	month;
		unsigned int	day;
	};

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
	std::pair<std::string, std::string>	splitLine(const std::string & line, const std::string & separator)
	{
		std::pair<std::string, std::string> line_pair;

		if (line.empty() || separator.empty())
			throw BitcoinExchange::InvalidLineException("empty");

		std::string::size_type separator_pos = line.find(separator);

		if (separator_pos == std::string::npos)
			throw BitcoinExchange::InvalidLineException("no separator found");

		line_pair.first = line.substr(0, separator_pos);
		line_pair.second = line.substr((separator_pos + separator.length()));

		return (line_pair);
	}

	/**
	 * @brief Validates the format of a date string.
	 * 
	 * Checks that the date string follows the format YYYY-MM-DD (10 characters total).
	 * The string must contain digits in positions 0-3, 5-6, and 8-9, with hyphens
	 * at positions 4 and 7.
	 * 
	 * @param date_str The date string to validate.
	 * 
	 * @throws BitcoinExchange::InvalidDateException If the string length is not 10
	 *         or if the format does not match YYYY-MM-DD pattern.
	 */
	void	validateDateFormat(const std::string & date_str)
	{
		if (date_str.length() != 10)
			throw BitcoinExchange::InvalidDateException("format: length");

		for (int i = 0; i < 10; ++i)
		{
			if (i == 4 || i == 7)
			{
				if (date_str[i] != '-')
					throw BitcoinExchange::InvalidDateException("format: separator");
			}
			else if (!std::isdigit(date_str[i]))
			{
				throw BitcoinExchange::InvalidDateException("format: digit");
			}
		}
	}

	/**
	 * @brief Parses a date string and extracts year, month, and day components.
	 *
	 * Expects a date string in the format "YYYY-MM-DD". Uses the extractDateNumber
	 * helper function to parse each numeric component separated by hyphens.
	 *
	 * @param date_str A reference to the date string to parse.
	 * @return s_date A structure containing the parsed year, month, and day.
	 *
	 * @throws BitcoinExchange::InvalidDateException if the format is invalid:
	 *         - Missing or misplaced hyphens between date components
	 *         - Unexpected characters after the day component
	 *
	 * @note The function assumes extractDateNumber will validate numeric values
	 *       and may throw exceptions for invalid numbers.
	 */
	s_date	parseDateString(const std::string & date_str)
	{
		s_date date;
		const char * c_str = date_str.c_str();

		validateDateFormat(date_str);

		date.year = static_cast<unsigned int>(std::strtol(c_str, NULL, 10));
		date.month = static_cast<unsigned int>(std::strtol((c_str + 5), NULL, 10));
		date.day = static_cast<unsigned int>(std::strtol((c_str + 8), NULL, 10));

		return (date);
	}

	/**
	 * @brief Determine whether a given year is a leap year.
	 *
	 * Implements the Gregorian leap year rules:
	 * - A year divisible by 400 is a leap year.
	 * - A year divisible by 100 (but not 400) is not a leap year.
	 * - A year divisible by 4 (but not 100) is a leap year.
	 *
	 * @param year The year to test (unsigned int).
	 * @return true if @p year is a leap year, false otherwise.
	 */
	bool	isLeapYear(unsigned int year)
	{
		if (year % 400 == 0)
			return (true);
		if (year % 100 == 0)
			return (false);
		if (year % 4 == 0)
			return (true);
		return (false);
	}

	/**
	 * @brief Get the maximum number of days for a given month.
	 *
	 * Determines the number of days in the month (1-12). For February (month == 2),
	 * the function accounts for leap years by querying isLeapYear(year). Months
	 * with 31 days: 1, 3, 5, 7, 8, 10, 12. Months with 30 days: 4, 6, 9, 11.
	 *
	 * @param month The month number (1 = January, ..., 12 = December).
	 * @param year The year used to determine leap years for February.
	 * @return The number of days in the specified month (28–31). Returns 0 for an invalid month value.
	 */
	unsigned int	getMaxDay(unsigned int month, unsigned int year)
	{
		switch (month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				return (31);
			case 4:
			case 6:
			case 9:
			case 11:
				return (30);
			case 2:
				return (isLeapYear(year) ? 29 : 28);
			default:
				return (0);
		}
	}

	/**
	 * @brief Validate a calendar date represented by an s_date structure.
	 *
	 * Ensures the month field is within the range [1, 12] and the day field is
	 * within the valid range for the specified month and year. The maximum valid
	 * day for the given month/year is determined via getMaxDay (which accounts
	 * for month lengths and leap years).
	 *
	 * @param date_str The date string to validate.
	 *
	 * @throws BitcoinExchange::InvalidDateException if the year is 0.
	 *
	 * @throws BitcoinExchange::InvalidDateException if the month is not in [1,12].
	 *
	 * @throws BitcoinExchange::InvalidDateException if the day is not in the range
	 *         [1, getMaxDay(month, year)].
	 */
	void	validateDate(const std::string & date_str)
	{
		s_date date = parseDateString(date_str);

		if (date.year < 1)
			throw BitcoinExchange::InvalidDateException("invalid year");

		if (date.month < 1 || date.month > 12)
			throw BitcoinExchange::InvalidDateException("invalid month");

		if (date.day < 1 || date.day > getMaxDay(date.month, date.year))
			throw BitcoinExchange::InvalidDateException("invalid day");
	}

	/**
	 * @brief Parses a string value into a double-precision floating-point number.
	 * 
	 * @param value_str The string to be parsed as a double value.
	 * 
	 * @return The parsed double value.
	 * 
	 * @throw BitcoinExchange::InvalidValueException if the string is empty.
	 * @throw BitcoinExchange::InvalidValueException if the first character is not a digit.
	 * @throw BitcoinExchange::InvalidValueException if the string contains invalid characters.
	 * @throw BitcoinExchange::InvalidValueException if the parsed value is out of range for double.
	 * 
	 * @note Uses std::strtod for conversion and validates the input string before and after parsing.
	 */
	double	parseValueString(const std::string & value_str)
	{
		if (value_str.empty())
			throw BitcoinExchange::InvalidValueException("empty");
		if (!std::isdigit(value_str[0]))
			throw BitcoinExchange::InvalidValueException("unexpected character in the beginning");

		char * endptr;
		errno = 0;
		double value = std::strtod(value_str.c_str(), &endptr);

		if (*endptr != '\0')
			throw BitcoinExchange::InvalidValueException("contains invalid character");
		if (errno == ERANGE)
			throw BitcoinExchange::InvalidValueException("value out of range");

		return (value);
	}

	/**
	 * @brief Parses a string value and validates it against a maximum threshold.
	 * 
	 * Converts a string representation of a numeric value to a double and ensures
	 * it does not exceed the specified maximum value.
	 * 
	 * @param value_str The string to parse into a double value
	 * @param max_value The maximum allowed value threshold
	 * 
	 * @return The parsed double value if valid
	 * 
	 * @throw BitcoinExchange::InvalidValueException If the parsed value exceeds max_value
	 * 
	 * @note This function delegates to another parseValueString(const std::string&)
	 *       overload for the actual string-to-double conversion
	 */
	double	parseValueString(const std::string & value_str, double max_value)
	{
		double value = parseValueString(value_str);
		if (value > max_value)
			throw BitcoinExchange::InvalidValueException("value out of range");
		return (value);
	}
}

// -------------------- ORTHODOX CANONICAL FORM -------------------- //

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

// -------------------- PUBLIC METHODS -------------------- //

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

namespace {
	std::string formatExceptionMsg(const std::string& defaultMsg, const std::string& msg) {
		if (msg.empty())
			return defaultMsg;
		return defaultMsg + ": " + msg;
	}
}

BitcoinExchange::InvalidLineException::InvalidLineException(const std::string & msg)
	: std::runtime_error(formatExceptionMsg("bad input", msg)) {}

BitcoinExchange::InvalidDateException::InvalidDateException(const std::string & msg)
	: std::runtime_error(formatExceptionMsg("bad date", msg)) {}

BitcoinExchange::InvalidValueException::InvalidValueException(const std::string & msg)
	: std::runtime_error(formatExceptionMsg("bad value", msg)) {}

BitcoinExchange::InvalidFileException::InvalidFileException(const std::string & msg)
	: std::runtime_error(formatExceptionMsg("bad file", msg)) {}
