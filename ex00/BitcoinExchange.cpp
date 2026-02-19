/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:19 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/19 19:29:24 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

/**
 * @brief default constructor
 *
 * Initializes the internal map of bitcoin exchange rates.
 * It reads them from a given .csv file. It also checks for correct
 * data in the file.
 *
 * Default database path = "./data.csv"
 */
BitcoinExchange::BitcoinExchange(void) :
	db_path_("./data.csv"),
	exchange_rates_()
{
	DEBUG_MSG("BitcoinExchange default constructed!");
}

/**
 * @brief parameterized constructor
 *
 * Same as default constructor, but initializes the database path
 * to 'db_path'.
 */
BitcoinExchange::BitcoinExchange(const std::string & db_path)
{
	//TODO
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange & other)
{
	//TODO
}

BitcoinExchange::~BitcoinExchange(void)
{
	//TODO
}

BitcoinExchange & BitcoinExchange::operator=(const BitcoinExchange & other)
{
	//TODO
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
void BitcoinExchange::parseDateString(const std::string & date)
{
	//TODO
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
 */
static void	evaluateDate(const std::string & date)
{
	//TODO
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
 */
double BitcoinExchange::getRate(const std::string & date)
{
	//TODO
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
 */
double	exchange(const std::string & date, double bitcoin_amount)
{
	//TODO
}
