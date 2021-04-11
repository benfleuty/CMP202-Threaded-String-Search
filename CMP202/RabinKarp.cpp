#include "RabinKarp.h"

#include <iostream>

RabinKarp::RabinKarp()
{
	text = ISearchAlgorithm::get_text();
	pattern = ISearchAlgorithm::get_pattern();
	type = algorithm_type::rabin_karp;
}

void RabinKarp::start_non_threaded_search()
{
	std::cout << "Rabin Karp non threaded search starting";
}

void RabinKarp::start_threaded_search()
{
	std::cout << "Rabin Karp threaded search starting";
}

void RabinKarp::start_search()
{
	if (threaded)
		start_threaded_search();
	else start_non_threaded_search();
}