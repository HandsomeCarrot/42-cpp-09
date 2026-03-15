#include "PmergeMe.hpp"
#include <exception>
#include <iostream>

int	main(int argc, char ** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		std::cout << "line : " << argv[i] << std::endl;
		try
		{
				PmergeMe p(argv[i]);
				
				p.sort();
		}
		catch (const std::exception & e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
	return (0);
}
