/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:10 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/19 19:25:57 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <map>
# include <string>

class BitcoinExchange
{
private:
	const std::string				db_path_;
	std::map<std::string, double>	exchange_rates_;
public:
	BitcoinExchange(void);
	BitcoinExchange(const std::string & db_path);
	BitcoinExchange(const BitcoinExchange & other);
	~BitcoinExchange(void);

	BitcoinExchange &	operator=(const BitcoinExchange & other);

	static void	parseDateString(const std::string & date);
	static void	evaluateDate(const std::string & date);

	double	getRate(const std::string & date);
	double	exchangeBitcoins(const std::string & date, double bitcoin_amount);
};

# define RESET	"\033[0m"
# define YELLOW	"\033[33m"

# ifdef DEBUG
#  define DEBUG_MSG(x) std::cout << YELLOW << "[DEBUG] " << RESET << x << std::endl
# else
#  define DEBUG_MSG(x)
# endif

#endif /* BITCOINEXCHANGE_HPP */
