// SECTION includes

	#include "PmergeMe.hpp"
	#include <exception>
	#include <iostream>

// END_SECTION includes

int	main(int argc, char ** argv)
{
	if (argc < 3)
	{
		std::cerr << "Error: wrong usage"
			<< "Usage: " << argv[0] << " <list of, at least 3, numbers>"
			<< std::endl;
		return (1);
	}

	try
	{
			PmergeMe p(argc - 1, argv + 1);
			p.sort();
			std::cout << p << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
