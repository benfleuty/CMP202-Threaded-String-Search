#include "ISearchAlgorithm.h"
#include <iostream>
#include <string>
#include "utils.h"

std::string ISearchAlgorithm::get_text()
{
	std::string input;
	load_file("search.txt", input);
	return input;


	
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	//std::string input;
	do
	{
		std::cout << "USE ASCII CHARACTERS ONLY\nYOU HAVE BEEN WARNED\nEnter y when the text you want to search is in a text file named \"search.txt\" in the same folder as this .exe\n";
		std::getline(std::cin, input);
	} while (input[0] != 'y');
	std::cout << "\nReading file...\n";
	try
	{
		load_file("search.txt", input);
	}
	catch (...)
	{
		die("Error reading search.txt");
	}
	return input;
};

std::string ISearchAlgorithm::get_pattern()
{
	bool complete = false;
	std::string pattern;

	while (!complete)
	{
		pattern = "";
		while (pattern.empty())
		{
			std::cout << "Enter the pattern you want to search for: ";
			std::getline(std::cin, pattern);
		}

		std::string input;
		while (tolower(input[0]) != 'y')
		{
			std::cout << "\nYou are searching for: " << pattern << "\n";
			std::cout << "Is this correct? [Y/n]: ";
			std::getline(std::cin, input);

			if (input.empty() || tolower(input[0]) == 'y')
			{
				complete = true;
				break;
			}

			if (input[0] == 'n')
				break;
		}
	}
	return pattern;
}

bool ISearchAlgorithm::is_threaded()
{
	while (true)
	{
		std::cout << "Do you want to run this search using multiple threads? [y/n] ";
		std::string in;
		std::getline(std::cin, in);
		char in_c = static_cast<char>(in[0]);
		if (in_c == 'y') return true;
		if (in_c == 'n') return false;
	}
}

ISearchAlgorithm::~ISearchAlgorithm() = default;