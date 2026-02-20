/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:10 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/20 13:39:52 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <map>
# include <stdexcept>
# include <string>
# include <utility>

class BitcoinExchange
{
// -------------------- VARIABLES -------------------- //
private:
	std::map<std::string, double>	db_; //more verbose name (e.g. database_, ...)?

// -------------------- ORTHODOX CANONICAL FORM -------------------- //
public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange & other);
	~BitcoinExchange(void);

	BitcoinExchange &	operator=(const BitcoinExchange & other);

// -------------------- EXTRAS -------------------- //
protected:
	struct s_date
	{
		unsigned int	year;
		unsigned int	month;
		unsigned int	day;
	};

	std::pair<std::string, std::string>	splitLine(const std::string & line, const std::string & separator);

	s_date	parseDateString(const std::string & date_str);
	void	validateDate(const s_date & date);

	double	parseValueString(const std::string & value_str);
	double	parseValueString(const std::string & value_str, double min_value, double max_value);

public:
	void	loadDatabase(const std::string & db_path);

	double	getRate(const std::string & date) const;

	double	exchange(const std::string & date, double bitcoin_amount) const;
	void	exchangeByFile(const std::string & file_path, const std::string & separator);

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
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* BITCOINEXCHANGE_HPP */
