#include "BoyerMoore.h"

#include <functional>
#include <iostream>
#include <thread>
#include <vector>

void BoyerMoore::start_non_threaded_search()
{
	std::cout << "Boyer Moore non threaded search starting" << std::endl;
	timer.start();
	matching_indexes = non_threaded_search();
	timer.stop();
}

// from cmp201
std::vector<long long> BoyerMoore::non_threaded_search() {
	//search_timer::start();
	const long long text_length = text.length();
	const long long pattern_length = pattern.length();

	std::vector<long long> matching_indexes;

	// lookup table to store how many places the given position should skip
	// ascii extended codes
	long long skip[256] = {};

	// set all points to be max skip value
	for (long long i = 0; i < 256; ++i)
		// Not in the pattern.
		skip[i] = pattern_length;

	// for each of the characters in the pattern
	for (long long i = 0; i < pattern_length; ++i)
		// set that character to its length from the end of the pattern
		skip[pattern[i]] = (pattern_length - 1) - i;

	// iterate through all the text, stopping patternLength positions from the end of the text
	for (long long i = 0; i <= text_length - pattern_length; ++i) {
		// check if the last character in the pattern is a match
		const long long pos = i + pattern_length - 1;
		const long long distance = skip[text[pos]];

		// if no match, skip by distance to the next position
		if (distance != 0) {
			i += distance - 1;
			continue;
		}

		// there is a match

		long long j;
		// iterate through the text to check each character
		for (j = 0; j < pattern_length; j++) {
			// if the current char in text being checked doesn't match that point in the pattern
			const long long local_pos = i + j;
			if (text[local_pos] != pattern[j]) break; // break and move on
		}

		// the word in text matches the pattern
		if (j == pattern_length)
			// add the index of the word to matchingIndexes
			matching_indexes.push_back(i);
	}
	//search_timer::stop();
	return matching_indexes;
}

void BoyerMoore::search_substring(unsigned long long start_pos, unsigned long long end_pos)
{
	std::vector<std::thread> write_threads;
	for (unsigned long long i = start_pos; i < end_pos - 1; ++i) {
		// check if the last character in the pattern is a match
		const unsigned long long pos = i + pattern.size() - 1;		// no need to lock this as it is a read
		const unsigned long long distance = skip_table[text[pos]];	// no need to lock this as it is a read

		// if no match, skip by distance to the next position
		if (distance != 0) {
			i += distance - 1;
			continue;
		}

		// execute the following when there is a match

		unsigned long long j;
		// iterate through the text to check each character
		for (j = 0; j < pattern.size(); ++j) {
			// if the current char in text being checked doesn't match that point in the pattern
			const unsigned long long local_pos = i + j;
			if (text[local_pos] != pattern[j]) break; // break and move on
		}

		// the word in text matches the pattern
		if (j == pattern.size()) {
			// add the index of the word to matching_indexes
			// CRITICAL POINT
			// must lock as writing to vector
			store_match_pos(i);
		}
	}
	std::cout << "\nThread finished: [" << start_pos << "] - [" << end_pos << "]\n";
}

void BoyerMoore::start_boyer_moore_search_threads(const unsigned int& search_thread_count)
{
	unsigned long long width = search_thread_width_;
	unsigned long long start_pos = 0;
	unsigned long long end_pos = 0;
	// start search threads X1 to Xn-2
	std::vector<std::thread> threads;
	std::thread match_replacer_thread(&BoyerMoore::replace_matches_in_text, this);

	for (unsigned int i = 1; i < search_thread_count; ++i)
	{
		// start_pos is the value of the search width multiplied by the number of threads that have already been started (i-1)
		start_pos = search_thread_width_ * (i - 1);
		// end_pos remains one tick ahead of start pos, leaving a width of search_thread_width_ between the start and end
		end_pos = search_thread_width_ * i;

		threads.emplace_back(std::thread(&BoyerMoore::search_substring, this, start_pos, end_pos));
	}

	// start search thread X-1
	start_pos = end_pos;
	end_pos = text.size();
	threads.emplace_back(std::thread(&BoyerMoore::search_substring, this, start_pos, end_pos));

	// join all threads
	for (auto& thread : threads) thread.join();
	search_complete_ = true;
	match_replacer_thread.join();
}

void BoyerMoore::start_threaded_search()
{
	std::cout << "Boyer Moore threaded search starting" << std::endl;

	timer.start();

	// Get the number of available threads on this CPU
	unsigned int num_of_threads = std::thread::hardware_concurrency();
	// num_of_threads = 0 if it cannot get the info
	while (num_of_threads == 0)
	{
		std::cout << "Your CPU count could not be automatically configured!\nPlease enter the number of threads your CPU can run concurrently: ";
		std::string input;
		std::getline(std::cin, input);
		// try to convert input from string to int
		try
		{
			num_of_threads = static_cast<unsigned int>(std::stoi(input));
		}
		catch (...) {}
	}

	const unsigned int num_of_search_threads = num_of_threads + 1;

	// if there are more threads than chars to search
	// don't bother with threading
	// as it'll probably take longer
	if (text.size() <= num_of_threads)
	{
		start_non_threaded_search();
	}

	// take the total length of the text and divide it by the number of available cores
	// the last search thread will take up the rest of the characters
	// e.g. 1999 chars spread over 15 threads = 133 positions on threads 1-14 and 138 on thread 15
	search_thread_width_ = text.size() / num_of_search_threads;

	std::cout << "\nConcurrent Threads: " << num_of_threads
		<< "\nSearch threads: " << num_of_search_threads
		<< "\nText size:" << text.size()
		<< "\nSearch thread width: " << search_thread_width_ << std::endl;

	start_boyer_moore_search_threads(num_of_search_threads);
	timer.stop();
}

BoyerMoore::BoyerMoore()
{
	type = algorithm_type::boyer_moore;
	for (auto& i : skip_table)
		i = pattern.size();

	// for each of the characters in the pattern
	for (unsigned long long i = 0; i < pattern.size(); ++i)
		// set that character to its length from the end of the pattern
		skip_table[pattern[i]] = pattern.size() - 1 - i;
}

void BoyerMoore::start_search()
{
	if (threaded)
		start_threaded_search();
	else start_non_threaded_search();
}