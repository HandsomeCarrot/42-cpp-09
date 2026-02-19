/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:10 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/19 22:20:54 by vpoka            ###   ########.fr       */
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
private:
	std::map<std::string, double>	db_; //more verbose name (e.g. database_, ...)?
public:
// ----- Orthodox Canonical Form ----- //
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange & other);
	~BitcoinExchange(void);

	BitcoinExchange &	operator=(const BitcoinExchange & other);

// ----- object bound functions ----- //

	void	loadDatabase(const std::string & db_path);

	double	getRate(const std::string & date) const;
	double	exchange(const std::string & date, double bitcoin_amount) const;

// ----- object independet functions ----- //

	static std::pair<std::string, std::string>	parseLine(const std::string & line, const std::string & separator);
	static void									parseDate(const std::string & date);
	static void									validateDate(const std::string & date);

// ----- custom exceptions ----- //

	class InvalidDateException : public std::runtime_error {};
	class InvalidValueException : public std::runtime_error {};
};

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* BITCOINEXCHANGE_HPP */
