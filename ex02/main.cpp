// SECTION includes
	#include "PmergeMe.hpp"
	#include <exception>
	#include <iostream>
// END_SECTION includes

int	main(int argc, char ** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		try
		{
				PmergeMe p(argv[i]);
				
				p.sort();
				
				std::cout << p << std::endl;
		}
		catch (const std::exception & e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
	return (0);
}
