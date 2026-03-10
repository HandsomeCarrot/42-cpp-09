#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <map>
# include <stdexcept>
# include <string>

class BitcoinExchange
{
// -------------------- VARIABLES -------------------- //
private:
	std::map<std::string, double>	db_;

// -------------------- ORTHODOX CANONICAL FORM -------------------- //
public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange & other);
	~BitcoinExchange(void);

	BitcoinExchange &	operator=(const BitcoinExchange & other);

// -------------------- EXTRAS -------------------- //
public:
	void	loadDatabase(const std::string & db_path);

	double	getRate(const std::string & date) const;

	double	exchange(const std::string & date, double bitcoin_amount) const;
	void	printExchangeByFile(const std::string & file_path, const std::string & separator);

// -------------------- EXCEPTIONS -------------------- //

	class InvalidLineException : public std::runtime_error
	{
	public:
		InvalidLineException(const std::string & msg = "");
	};

	class InvalidDateException : public std::runtime_error
	{
	public:
		InvalidDateException(const std::string & msg = "");
	};

	class InvalidValueException : public std::runtime_error
	{
	public:
		InvalidValueException(const std::string & msg = "");
	};

	class InvalidFileException : public std::runtime_error
	{
	public:
		InvalidFileException(const std::string & msg = "");
	};
};

// -------------------- DEBUGGING STUFF -------------------- //

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  include <iostream>
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* BITCOINEXCHANGE_HPP */
