/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:46:06 by vpoka             #+#    #+#             */
/*   Updated: 2026/02/19 19:32:16 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <exception>
#include <stdexcept>

/**
 * @brief saves input file data */
/*static void saveInputFileData(const char * file_path)
{
	(void)file_path;
}*/

int	main(int argc, char ** argv)
{
	(void)argv;
	try
	{
		if (argc != 2)
			throw std::runtime_error("expected exactly one file path argument.");
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}