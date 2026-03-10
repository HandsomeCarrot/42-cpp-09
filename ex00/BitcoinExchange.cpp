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
	 * @brief Split a line around the first occurrence of a separator.
	 *
	 * This helper searches @p line for the first occurrence of @p separator and
	 * returns the substrings located before and after it. The separator itself is
	 * not included in the returned pair.
	 *
	 * @param line Input line to split. Must not be empty.
	 * @param separator Delimiter used to split the line. Must not be empty.
	 *
	 * @return A pair whose first element contains the text before the separator
	 *         and whose second element contains the text after it.
	 *
	 * @throws BitcoinExchange::InvalidLineException, if the separator does not occur in
	 *         @p line.
	 * 
	 * @throws std::runtime_error, if @p line/separator is empty.
	 *
	 * @note If the separator appears more than once, only the first occurrence is
	 *       used.
	 */
	std::pair<std::string, std::string>	splitLine(const std::string & line, const std::string & separator)
	{
		std::pair<std::string, std::string> line_pair;

		if (line.empty() || separator.empty())
			throw std::runtime_error("splitLine(): incorrect usage");

		std::string::size_type separator_pos = line.find(separator);

		if (separator_pos == std::string::npos)
			throw BitcoinExchange::InvalidLineException("'" + line + "': no separator (" + separator + ") found");

		line_pair.first = line.substr(0, separator_pos);
		line_pair.second = line.substr((separator_pos + separator.length()));

		return (line_pair);
	}

	/**
	 * @brief Validate the lexical format of a date string.
	 *
	 * The accepted format is exactly `YYYY-MM-DD`: ten characters long, decimal
	 * digits for the year, month, and day fields, and `'-'` characters at
	 * positions 4 and 7. This function only validates the textual layout; it does
	 * not verify that the resulting date is calendar-valid.
	 *
	 * @param date_str Date string to validate.
	 *
	 * @throws BitcoinExchange::InvalidDateException If @p date_str does not match
	 *         the required `YYYY-MM-DD` format.
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
	 * @brief Parse a formatted date string into numeric components.
	 *
	 * This function first calls `validateDateFormat()` and then converts the
	 * `YYYY`, `MM`, and `DD` fields to unsigned integers with `std::strtoul()`.
	 * Since the format check guarantees fixed positions and digit-only fields,
	 * the numeric conversions are expected to succeed without partial parsing.
	 *
	 * @param date_str Date string in `YYYY-MM-DD` format.
	 *
	 * @return An `s_date` structure containing the parsed year, month, and day.
	 *
	 * @throws BitcoinExchange::InvalidDateException Propagated from
	 *         `validateDateFormat()` when the input format is invalid.
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
	 * @brief Return whether a year is a Gregorian leap year.
	 *
	 * @param year Year to test.
	 *
	 * @return `true` if @p year is divisible by 400, or divisible by 4 but not
	 *         by 100; `false` otherwise.
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
	 * @brief Return the maximum valid day number for a month.
	 *
	 * February is adjusted according to `isLeapYear(year)`. Invalid month values
	 * are not rejected with an exception; they yield `0` so callers can treat
	 * them as invalid through their own checks.
	 *
	 * @param month Month number in the range 1 to 12.
	 * @param year Year associated with the date, used only for February.
	 *
	 * @return The number of days in the specified month, or `0` if @p month is
	 *         outside the range 1 to 12.
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
	 * @brief Validate that a date string is both well-formed and calendar-valid.
	 *
	 * Validation is performed in two stages:
	 * - `parseDateString()` ensures the input matches the required
	 *   `YYYY-MM-DD` layout and extracts numeric fields.
	 * - The parsed components are then checked for semantic validity:
	 *   year must be non-zero, month must be in the range 1 to 12, and day
	 *   must be in the range 1 to `getMaxDay(month, year)`.
	 *
	 * @param date_str Date string to validate.
	 *
	 * @throws BitcoinExchange::InvalidDateException If the format is invalid or
	 *         if the represented date does not exist.
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
	 * @brief Parse a numeric string as a `double`.
	 *
	 * Conversion is delegated to `std::strtod()`. The function requires the whole
	 * input string to be consumed successfully and rejects values that overflow or
	 * underflow according to `errno == ERANGE`.
	 *
	 * @param value_str String representation of the numeric value.
	 *
	 * @return The parsed floating-point value.
	 *
	 * @throws BitcoinExchange::InvalidValueException If @p value_str is empty, if
	 *         it contains trailing or otherwise invalid characters, or if the
	 *         conversion reports a range error.
	 *
	 * @note This function does not enforce semantic constraints such as
	 *       non-negativity or upper bounds. Those checks are performed by the
	 *       caller where needed.
	 */
	double	parseValueString(const std::string & value_str)
	{
		if (value_str.empty())
			throw BitcoinExchange::InvalidValueException("empty value string");

		char * endptr;
		errno = 0;
		double value = std::strtod(value_str.c_str(), &endptr);

		if (*endptr != '\0')
			throw BitcoinExchange::InvalidValueException(value_str + ": value string contains invalid character");
		if (errno == ERANGE)
			throw BitcoinExchange::InvalidValueException(value_str + ": value out of range");

		return (value);
	}

	/**
	 * @brief Open an input file stream and report failures as domain exceptions.
	 *
	 * @param file_path Path of the file to open.
	 * @param file Stream object that will be opened.
	 *
	 * @throws BitcoinExchange::InvalidFileException If @p file_path is empty or if
	 *         the file cannot be opened.
	 *
	 * @note The stream state is modified by this function. Ownership and closing
	 *       of the stream remain the caller's responsibility.
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
	 * @brief Read and validate the first line of a delimited text file.
	 *
	 * The function reads one line from @p file, splits it with @p separator, and
	 * compares the resulting two fields with the expected column names stored in
	 * @p columns.
	 *
	 * @param file Open input stream positioned at the beginning of the file.
	 * @param separator Field separator expected in the header line.
	 * @param columns Expected header fields as `{first_column, second_column}`.
	 *
	 * @throws BitcoinExchange::InvalidFileException If the file is empty, if the
	 *         first line cannot be read correctly, or if the header cannot be
	 *         split with the requested separator.
	 * @throws BitcoinExchange::InvalidLineException If the header is syntactically
	 *         split successfully but its field names differ from @p columns.
	 *
	 * @note On success, the next read from @p file starts at the second line.
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
	 * @brief Parse a data line into a validated date/value pair.
	 *
	 * The line is split with @p separator, the left field is validated as a
	 * calendar date, and the right field is parsed as a floating-point value.
	 *
	 * @param line Input record to parse.
	 * @param separator Field separator expected in @p line.
	 *
	 * @return A pair containing the validated date string and the parsed value.
	 *
	 * @throws BitcoinExchange::InvalidLineException If the line cannot be split.
	 * @throws BitcoinExchange::InvalidDateException If the date field is invalid.
	 * @throws BitcoinExchange::InvalidValueException If the value field cannot be
	 *         parsed as a `double`.
	 */
	std::pair<std::string, double> parseCSVLine(const std::string & line, const std::string & separator)
	{
		std::pair<std::string, std::string> split_line = splitLine(line, separator);
		
		validateDate(split_line.first);

		double value = parseValueString(split_line.second);

		return (std::make_pair(split_line.first, value));
	}

	/**
	 * @brief Verify that a file-reading loop terminated normally.
	 *
	 * This helper distinguishes a normal end-of-file condition from an I/O error
	 * or another unexpected stream failure after the caller finishes reading from
	 * @p file.
	 *
	 * @param file Input stream to inspect.
	 *
	 * @throws BitcoinExchange::InvalidFileException If the stream is in a bad
	 *         state or if reading stopped without reaching end-of-file.
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
 * @brief Construct an empty exchange database.
 */
BitcoinExchange::BitcoinExchange(void) :
	db_()
{
	DEBUG_MSG("BitcoinExchange default constructed.");
}

/**
 * @brief Construct a copy of another `BitcoinExchange`.
 *
 * @param other Source object whose internal database is copied.
 */
BitcoinExchange::BitcoinExchange(const BitcoinExchange & other) :
	db_(other.db_)
{
	DEBUG_MSG("BitcoinExchange copy constructed.");
}

/**
 * @brief Destroy the `BitcoinExchange` instance.
 */
BitcoinExchange::~BitcoinExchange(void)
{
	DEBUG_MSG("BitcoinExchange deconstructed.");
}

/**
 * @brief Replace this object's state with another instance's state.
 *
 * @param other Source object to copy from.
 *
 * @return `*this`.
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
 * @brief Load historical exchange rates from the database CSV file.
 *
 * The file is expected to use the header `date,exchange_rate`. Each non-empty
 * data line is parsed as a `YYYY-MM-DD` date associated with a floating-point
 * exchange rate. Parsed entries are inserted into `db_` using the date string
 * as the key.
 *
 * @param file_path Path to the CSV database file.
 *
 * @throws InvalidFileException If the file cannot be opened or fully read.
 * @throws InvalidLineException If a non-empty line cannot be split correctly.
 * @throws InvalidDateException If a date field is malformed or invalid.
 * @throws InvalidValueException If an exchange rate cannot be parsed or is
 *         negative.
 *
 * @note Empty lines are ignored and reported as warnings on `std::cerr`.
 * @note When a date appears multiple times, the last value read replaces the
 *       previous one because insertion is done through `db_[key]`.
 * @note Existing contents of `db_` are not cleared before loading; successfully
 *       parsed entries are added to or overwrite entries in the current map.
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
			
			if (entry.second < 0)
				throw InvalidValueException("negative exchange rate.");
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
 * @brief Return the exchange rate applicable to a given date.
 *
 * The requested date must be valid. If the exact date exists in `db_`, its rate
 * is returned. Otherwise, the function returns the rate associated with the
 * closest earlier date present in the database.
 *
 * @param date Requested date in `YYYY-MM-DD` format.
 *
 * @return The rate for @p date, or for the closest previous available date.
 *
 * @throws std::runtime_error If the internal database is empty.
 * @throws InvalidDateException If @p date is malformed, calendar-invalid, or
 *         older than the earliest date stored in the database.
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
 * @brief Convert a Bitcoin amount to its value for a given date.
 *
 * This function first validates the requested amount, then obtains the
 * applicable rate through `getRate(date)`, and finally returns
 * `bitcoin_amount * rate`.
 *
 * @param date Date for which the rate should be applied.
 * @param bitcoin_amount Bitcoin amount to evaluate.
 *
 * @return The converted value for the given amount and date.
 *
 * @throws std::runtime_error If @p bitcoin_amount is negative or greater than
 *         `1000`.
 * @throws InvalidDateException Propagated from `getRate()` when @p date is
 *         invalid or predates the available database range.
 */
double BitcoinExchange::exchange(const std::string & date, double bitcoin_amount) const
{
	if (bitcoin_amount < 0)
		throw std::runtime_error("negative bitcoin amount");
	else if (bitcoin_amount > 1000)
		throw std::runtime_error("bitcoin amount > 1000.");

	return (getRate(date) * bitcoin_amount);
}

/**
 * @brief Read an input file of exchange requests and print each result.
 *
 * The input file must begin with a header matching `date` and `value`,
 * separated by @p separator. Each subsequent non-empty line is parsed with
 * `parseCSVLine()`, converted through `exchange()`, and printed in the form:
 * `date => value = result`.
 *
 * Errors affecting individual data lines are reported to `std::cerr` and do not
 * stop processing of later lines. Failures that prevent processing the file as a
 * whole are wrapped and rethrown as `std::runtime_error`.
 *
 * @param file_path Path to the input request file.
 * @param separator Separator expected between the date and value columns.
 *
 * @throws std::runtime_error If the file cannot be opened, if its header is
 *         invalid, if a stream-level read failure occurs, or if another
 *         unrecoverable exception escapes the per-line processing block.
 *
 * @note Empty data lines are treated as line-level errors and are skipped.
 */
void BitcoinExchange::printExchangeByFile(const std::string & file_path, const std::string & separator)
{
	try
	{
		std::ifstream	file_stream;
		std::string		line;
		int				line_number = 0;

		openFile(file_path, file_stream);
		
		validateCSVHeader(file_stream, separator, std::make_pair("date", "value"));

		while (std::getline(file_stream, line))
		{
			line_number++;

			if (line.empty())
			{
				std::cerr << "Error: input file: line " << line_number << ": empty line" << std::endl;
				continue ;
			}

			try
			{
				std::pair<std::string, double>	entry = parseCSVLine(line, separator);
				double							exchanged_bitcoins = exchange(entry.first, entry.second);

				std::cout << entry.first << " => " << entry.second << " = " << exchanged_bitcoins << std::endl;
			}
			catch (const std::exception & e)
			{
				std::cerr << "Error: input file: line " << line_number << ": " << e.what() << std::endl;
				continue ;
			}
		}
		checkFileEnd(file_stream);
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(std::string("exchanging by file: unexpected exception occured: ") + e.what());
	}
}

// -------------------- EXCEPTIONS -------------------- //

/**
 * @brief Construct an exception describing an invalid input line.
 *
 * @param msg Error message exposed through `what()`.
 */
BitcoinExchange::InvalidLineException::InvalidLineException(const std::string & msg)
	: std::runtime_error(msg) {}

/**
 * @brief Construct an exception describing an invalid date.
 *
 * @param msg Error message exposed through `what()`.
 */
BitcoinExchange::InvalidDateException::InvalidDateException(const std::string & msg)
	: std::runtime_error(msg) {}

/**
 * @brief Construct an exception describing an invalid numeric value.
 *
 * @param msg Error message exposed through `what()`.
 */
BitcoinExchange::InvalidValueException::InvalidValueException(const std::string & msg)
	: std::runtime_error(msg) {}

/**
 * @brief Construct an exception describing a file-related failure.
 *
 * @param msg Error message exposed through `what()`.
 */
BitcoinExchange::InvalidFileException::InvalidFileException(const std::string & msg)
	: std::runtime_error(msg) {}
