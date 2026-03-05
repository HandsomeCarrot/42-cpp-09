/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:19 by vpoka             #+#    #+#             */
/*   Updated: 2026/03/05 17:50:58 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <cctype>		// std::isdigit()
#include <cerrno>		// errno, ERANGE
#include <cstdlib>		// std::strtoul(), std::strtod()
#include <cstring>		// std::strerror()
#include <exception>	// std::exception
#include <fstream>		// std::ifstream
#include <iostream>		// std::cerr, BitcoinExchange::loadDatabase()
#include <sstream>		// std::stringstream, BitcoinExchange::loadDatabase()
#include <stdexcept>	// std::runtime_error
#include <string>		// std::string, std::getline()
#include <utility>		// std::pair, std::make_pair()

// -------------------- HELPER FUNCTIONS -------------------- //

namespace
{
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
			throw BitcoinExchange::InvalidLineException("'" + line + "': no separator (" + separator + ") found");

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
			throw BitcoinExchange::InvalidDateException("'" + date_str + "': format: length");

		for (int i = 0; i < 10; ++i)
		{
			if (i == 4 || i == 7)
			{
				if (date_str[i] != '-')
					throw BitcoinExchange::InvalidDateException("'" + date_str + "': format: separator");
			}
			else if (!std::isdigit(static_cast<unsigned char>(date_str[i])))
			{
				throw BitcoinExchange::InvalidDateException("'" + date_str + "': format: digit");
			}
		}
	}

	/**
	 * @brief Parses a date string and extracts year, month, and day components.
	 *
	 * Expects a date string in the format "YYYY-MM-DD". First validates the
	 * format using validateDateFormat(), which checks for correct length and
	 * digit/hyphen positioning. After validation, converts the date components
	 * to unsigned integers using std::strtoul.
	 *
	 * @param date_str A reference to the date string to parse.
	 * @return s_date A structure containing the parsed year, month, and day.
	 *
	 * @throws BitcoinExchange::InvalidDateException if the format is invalid
	 *         (delegated from validateDateFormat).
	 */
	s_date	parseDateString(const std::string & date_str)
	{
		s_date date;
		const char * c_str = date_str.c_str();

		validateDateFormat(date_str);

		date.year = static_cast<unsigned int>(std::strtoul(c_str, NULL, 10));
		date.month = static_cast<unsigned int>(std::strtoul((c_str + 5), NULL, 10));
		date.day = static_cast<unsigned int>(std::strtoul((c_str + 8), NULL, 10));

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
	 * @brief Validates a date string for format and logical correctness.
	 *
	 * Parses the date string using parseDateString() and validates that:
	 * - The year is not 0
	 * - The month is within the range [1, 12]
	 * - The day is within the valid range [1, getMaxDay(month, year)]
	 * 
	 * The maximum valid day for the given month/year is determined via getMaxDay,
	 * which accounts for month lengths and leap years.
	 *
	 * @param date_str The date string in YYYY-MM-DD format to validate.
	 *
	 * @throws BitcoinExchange::InvalidDateException if the date format is invalid,
	 *         year is 0, month is not in [1,12], or day is not in the valid range.
	 */
	void	validateDate(const std::string & date_str)
	{
		s_date date = parseDateString(date_str);

		if (date.year == 0)
			throw BitcoinExchange::InvalidDateException("'" + date_str + "': invalid year");

		if (date.month == 0 || date.month > 12)
			throw BitcoinExchange::InvalidDateException("'" + date_str + "': invalid month");

		if (date.day == 0 || date.day > getMaxDay(date.month, date.year))
			throw BitcoinExchange::InvalidDateException("'" + date_str + "': invalid day");
	}

	/**
	 * @brief Parses a string value into a double-precision floating-point number.
	 * 
	 * Uses std::strtod for conversion and validates that the entire string is
	 * consumed (no trailing invalid characters), the value is non-negative,
	 * and within the valid range for double.
	 * 
	 * @param value_str The string to be parsed as a double value.
	 * 
	 * @return The parsed non-negative double value.
	 * 
	 * @throw BitcoinExchange::InvalidValueException if the string is empty.
	 * @throw BitcoinExchange::InvalidValueException if the string contains invalid characters.
	 * @throw BitcoinExchange::InvalidValueException if the value is negative.
	 * @throw BitcoinExchange::InvalidValueException if the parsed value causes ERANGE.
	 */
	double	parseValueString(const std::string & value_str)
	{
		if (value_str.empty())
			throw BitcoinExchange::InvalidValueException(value_str + ": empty");

		char * endptr;
		errno = 0;
		double value = std::strtod(value_str.c_str(), &endptr);

		if (*endptr != '\0')
			throw BitcoinExchange::InvalidValueException(value_str + ": contains invalid character");
		if (value < 0)
			throw BitcoinExchange::InvalidValueException(value_str + ": value not positive");
		if (errno == ERANGE)
			throw BitcoinExchange::InvalidValueException(value_str + ": value out of range");

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
			throw BitcoinExchange::InvalidValueException(value_str + ": value out of range");
		return (value);
	}

	/**
	 * @brief Opens a file stream and validates the operation.
	 * 
	 * @param file_path The path to the file to open. Must not be empty.
	 * @param file Reference to an ifstream object that will be used to open the file.
	 * 
	 * @throws BitcoinExchange::InvalidFileException if file_path is empty.
	 * @throws BitcoinExchange::InvalidFileException if the file cannot be opened (with errno details).
	 * 
	 * @note The file stream must be closed by the caller when done.
	 */
	void openFile(const std::string & file_path, std::ifstream & file)
	{
		int tmp_errno;
		
		if (file_path.empty())
			throw BitcoinExchange::InvalidFileException("no file provided");

		errno = 0;
		file.open(file_path.c_str());

		tmp_errno = errno;
		if (!file.is_open())
			throw BitcoinExchange::InvalidFileException(std::string("failed to open: ") + std::strerror(tmp_errno));
	}

	/**
	 * @brief Validates the header line of a CSV file.
	 * 
	 * Reads the first line of the file and checks that it matches the expected
	 * column format. The header must contain two columns separated by the specified
	 * separator, matching the provided column names.
	 * 
	 * @param file Reference to an open input file stream positioned at the start.
	 * @param separator The delimiter string used to separate columns (e.g., "," or " | ").
	 * @param columns A pair containing the expected column names (first, second).
	 * 
	 * @throws BitcoinExchange::InvalidFileException if file reading fails.
	 * @throws BitcoinExchange::InvalidFileException if the file is empty.
	 * @throws BitcoinExchange::InvalidFileException if the header format is invalid.
	 * @throws BitcoinExchange::InvalidFileException if column names don't match the expected values.
	 * 
	 * @note After this call, the file stream will be positioned at the second line.
	 */
	void validateCSVHeader(
		std::ifstream & file,
		const std::string & separator,
		const std::pair<std::string, std::string> & columns)
	{
		// -- read the first line --
		int tmp_errno;
		std::string header_line;
		
		errno = 0;
		std::getline(file, header_line);

		tmp_errno = errno;
		if (!file && !file.eof())
			throw BitcoinExchange::InvalidFileException(std::string("error while reading: ") + std::strerror(tmp_errno));
		if (header_line.empty())
			throw BitcoinExchange::InvalidFileException("empty file");

		// -- validate header --
		std::pair<std::string, std::string> split_line;

		try
		{
			split_line = splitLine(header_line, separator);
		}
		catch (const BitcoinExchange::InvalidLineException & e)
		{
			throw BitcoinExchange::InvalidFileException(std::string("invalid csv header: ") + e.what());
		}

		if (split_line != columns)
			throw BitcoinExchange::InvalidLineException("invalid column descriptors: expected: '" + columns.first + "," + columns.second + "'");
	}

	/**
	 * @brief Parses a CSV line into a date-value pair.
	 * 
	 * Splits the line using the specified separator, validates the date format,
	 * and parses the value as a double. The line is expected to contain a date
	 * in YYYY-MM-DD format followed by a numeric value.
	 * 
	 * @param line The CSV line to parse.
	 * @param separator The delimiter string used to split the line.
	 * 
	 * @return A pair containing the date string and the parsed numeric value.
	 * 
	 * @throws BitcoinExchange::InvalidLineException if line format is invalid.
	 * @throws BitcoinExchange::InvalidDateException if the date is invalid.
	 * @throws BitcoinExchange::InvalidValueException if the value cannot be parsed.
	 */
	std::pair<std::string, double> parseCSVLine(const std::string & line, const std::string & separator)
	{
		std::pair<std::string, std::string> split_line = splitLine(line, separator);
		
		validateDate(split_line.first);

		double value = parseValueString(split_line.second);

		return (std::make_pair(split_line.first, value));
	}

	/**
	 * @brief Validates that a file stream has reached EOF without errors.
	 * 
	 * Checks the state of a file stream after reading operations to ensure
	 * it reached the end-of-file naturally without encountering errors.
	 * 
	 * @param file The input file stream to check.
	 * 
	 * @throws BitcoinExchange::InvalidFileException
	 *         - stream is in a bad state (with errno details),
	 *         - failed without reaching EOF.
	 * 
	 * @note This should be called after a read loop completes to verify
	 *       the file was read successfully in its entirety.
	 */
	void checkFileEnd(const std::ifstream & file)
	{
		int tmp_errno = errno;

		if (file.bad())
			throw BitcoinExchange::InvalidFileException(std::string("error while reading: ") + std::strerror(tmp_errno));
		else if (!file.eof())
			throw BitcoinExchange::InvalidFileException("error while reading: unexpected read failure");
	}
}

// -------------------- ORTHODOX CANONICAL FORM -------------------- //

/**
 * @brief Default constructor.
 */
BitcoinExchange::BitcoinExchange(void) :
	db_()
{
	DEBUG_MSG("BitcoinExchange default constructed.");
}

/**
 * @brief Copy constructor.
 * 
 * @param other The BitcoinExchange object to copy from.
 */
BitcoinExchange::BitcoinExchange(const BitcoinExchange & other) :
	db_(other.db_)
{
	DEBUG_MSG("BitcoinExchange copy constructed.");
}

/**
 * @brief Destructor.
 */
BitcoinExchange::~BitcoinExchange(void)
{
	DEBUG_MSG("BitcoinExchange deconstructed.");
}

/**
 * @brief Copy assignment operator.
 * 
 * @param other The BitcoinExchange object to assign from.
 * 
 * @return Reference to this object after assignment.
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

/**
 * @brief Loads exchange rate data from a CSV file into the database.
 * 
 * Opens and parses a CSV file containing date and exchange rate pairs.
 * The file must have a header with columns "date" and "exchange_rate",
 * separated by commas. Each subsequent line should contain a date in
 * YYYY-MM-DD format and a corresponding numeric exchange rate value.
 * Empty lines are skipped.
 * 
 * @param file_path Path to the CSV file containing exchange rate data.
 * 
 * @throws InvalidFileException if the file cannot be opened, has invalid
 *         format, contains invalid dates/values, or encounters read errors.
 * 
 * @note The database (db_) is populated with date-rate pairs as a std::map.
 *       If duplicate dates exist in the file, later entries will overwrite earlier ones.
 */
void BitcoinExchange::loadDatabase(const std::string & file_path)
{
	std::string			line;
	std::ifstream		file;
	unsigned int		file_line_number = 0;
	std::stringstream	stream;

	try
	{
		openFile(file_path, file);
		validateCSVHeader(file, ",", std::make_pair("date", "exchange_rate"));
		file_line_number++;

		while (std::getline(file, line))
		{
			file_line_number++;
			if (line.empty())
			{
				std::cerr << "Warning: database: line " << file_line_number << " is empty!" << std::endl;
				continue ;
			}

			std::pair<std::string, double> entry = parseCSVLine(line, ",");
			db_[entry.first] = entry.second;
		}

		checkFileEnd(file);
	}
	catch (const InvalidFileException & e)
	{
		stream << "database file: " << e.what();
		throw InvalidFileException(stream.str());
	}
	catch (const InvalidLineException & e)
	{
		stream << "line " << file_line_number << " in database: " << e.what();
		throw InvalidLineException(stream.str());
	}
	catch (const InvalidValueException & e)
	{
		stream << "exchange rate in database: line " << file_line_number << ": " << e.what();
		throw InvalidValueException( stream.str());
	}
	catch (const InvalidDateException & e)
	{
		stream << "date in database: line " << file_line_number << ": " << e.what();
		throw InvalidDateException(stream.str());
	}
	catch (const std::exception & e)
	{
		stream << "unexpected exception during database loading: " << e.what();
		std::runtime_error(stream.str());
	}
}

/**
 * @brief Retrieves the Bitcoin exchange rate for a given date.
 * 
 * Looks up the exchange rate in the database for the specified date.
 * If an exact match is not found, returns the rate from the most recent
 * date before the requested date.
 * 
 * @param date The date string in the format expected by the database
 *             ("YYYY-MM-DD").
 * 
 * @return The Bitcoin exchange rate as a double for the given date
 *         or the most recent available date before it.
 * 
 * @throw std::runtime_error If the database is empty.
 * @throw InvalidDateException If the requested date is older than
 *                             the earliest date in the database.
 * 
 * @note The date validation is performed by validateDate() method.
 */
double BitcoinExchange::getRate(const std::string & date) const
{
	if (db_.empty())
		throw std::runtime_error("database is empty");

	validateDate(date);

	std::map<std::string, double>::const_iterator i = db_.upper_bound(date);

	if (i != db_.begin())
		--i;

	if (i->first > date)
	{
		throw InvalidDateException("date is too old");
	}

	return (i->second);
}

/**
 * @brief Calculates the exchange value of a given amount of Bitcoin for a specified date.
 * 
 * @param date A string representing the date for which the exchange rate should be retrieved.
 * @param bitcoin_amount The amount of Bitcoin to exchange. Must be non-negative.
 * 
 * @return The exchange value calculated by multiplying the Bitcoin amount with the 
 *         exchange rate retrieved for the given date.
 * 
 * @throw std::runtime_error if bitcoin_amount is negative.
 */
double BitcoinExchange::exchange(const std::string & date, double bitcoin_amount) const
{
	if (bitcoin_amount < 0)
		throw std::runtime_error("invalid bitcoin amount");

	return (getRate(date) * bitcoin_amount);
}

/**
 * @brief ...
 */
void BitcoinExchange::exchangeByFile(const std::string & file_path, const std::string & separator)
{
	//TODO
	(void)file_path;
	(void)separator;
	(void)parseValueString("", 1000); //delete me
}

// -------------------- EXCEPTIONS -------------------- //

BitcoinExchange::InvalidLineException::InvalidLineException(const std::string & msg)
	: std::runtime_error(msg) {}

BitcoinExchange::InvalidDateException::InvalidDateException(const std::string & msg)
	: std::runtime_error(msg) {}

BitcoinExchange::InvalidValueException::InvalidValueException(const std::string & msg)
	: std::runtime_error(msg) {}

BitcoinExchange::InvalidFileException::InvalidFileException(const std::string & msg)
	: std::runtime_error(msg) {}
