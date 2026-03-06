#include "BitcoinExchange.hpp"
#include <iostream>

int	main(int argc, char ** argv)
{
	try
	{
		if (argc != 2)
			throw std::runtime_error("expected exactly one argument. (input file path)");

		BitcoinExchange btc;
		btc.loadDatabase("data.csv");

		std::cout << "INFO: output format: exchange date => bitcoin amount = monetary value" << std::endl;

		btc.printExchangeByFile(argv[1], " | ");
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}