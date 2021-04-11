#include <iostream>
#include <string>
#include <filesystem>

#include "BoyerMoore.h"
#include "RabinKarp.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
enum class algorithm
{
	not_set,
	boyer_moore,
	rabin_karp
};

void show_welcome_menu()
{
	cout << "Welcome" << endl << "Please choose which algorithm you would like to use: " << endl <<
		"1) Boyer Moore" << endl <<
		"2) Rabin Karp" << endl << "[1-2]: ";
}

algorithm get_desired_algorithm(const char& input)
{
	switch (input)
	{
	case '1':
		return algorithm::boyer_moore;
	case '2':
		return algorithm::rabin_karp;
	default:
		return algorithm::not_set;
	}
};

bool confirm_algorithm(const algorithm& algo)
{
	const string out = (algo == algorithm::boyer_moore) ? "Boyer-Moore" : "Rabin-Karp";
	while (true)
	{
		cout << "You have selected " << out << "." << endl << "Is this correct? [Y/n] ";
		string input;
		std::getline(cin, input);

		// take no input to mean yes
		if (input.empty()) return true;

		const char c = tolower(input[0]);
		if (c == 'y') return true;
		if (c == 'n') return false;
	}
}

int main()
{
	while (true) {
		// display options to the user
		// and get their desired search algorithm_type
		algorithm algo = algorithm::not_set;
		while (algo == algorithm::not_set)
		{
			system("cls");
			show_welcome_menu();

			string input;
			std::getline(cin, input);
			auto in = static_cast<char>(tolower(input[0]));

			if (in == 'q') exit(0);
			if (in != '1' && in != '2') continue;

			algo = get_desired_algorithm(in);

			if (!confirm_algorithm(algo))
				algo = algorithm::not_set;
		}
		// algorithm_type has been selected

		SearchAlgorithm* searcher = nullptr;

		if (algo == algorithm::boyer_moore) searcher = new BoyerMoore();
		else if (algo == algorithm::rabin_karp) searcher = new RabinKarp();
		else {
			system("cls");
			cout << "Looks like there was an error! Let's try again..." << endl;
			system("pause");
			continue;
		}

		searcher->start_search();
		searcher->output_search_results();

		bool again = false;
		do
		{
			cout << "\nSearch again?[y/n] ";
			string in;
			std::getline(std::cin, in);
			char input = tolower(in[0]);

			if (input == 'y') {
				again = true;
				break;
			}
			if (input == 'n' || input == 'q') {
				again = false;
				break;
			}
		} while (true);
		if (again) continue;
		return 0;
	}
}