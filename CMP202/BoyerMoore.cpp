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
	std::string report = generate_report();
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
	for (long long i = 0; i < text_length - pattern_length; ++i) {
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
	std::cout << "this thread ran";
	return;
	// iterate through all the text in the subset
	for (unsigned long long i = start_pos; i < end_pos; ++i) {
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
			// add the index of the word to matchingIndexes
			// CRITICAL POINT
			// must lock as writing to vector
			std::unique_lock<std::mutex> lock(matching_indexes_mutex);
			matching_indexes.push_back(i);
			progress_ready_ = true;
			progress_cv.notify_one();
		}
	}
}

void BoyerMoore::start_boyer_moore_search_threads(const unsigned int& search_thread_count)
{
	unsigned long long width = search_thread_width_;
	unsigned long long start_pos = 0;
	unsigned long long end_pos = 0;
	// start search threads X1 to Xn-1
	for (unsigned int i = 1; i < search_thread_count; ++i)
	{
		// start_pos is the value of the search width multiplied by the number
		// of threads that have already been started (i-1)
		start_pos = search_thread_width_ * (i - 1);
		// end_pos remains one tick ahead of start pos, leaving a width of search_thread_width_ between the start and end
		end_pos = search_thread_count * i;

		/* As the lecture teaches */
		// error: no instance of constructor "std::thread::thread" matches the argument lis
		//std::thread temp_thread(search_substring, start_pos, end_pos);

		// SO attempt 1 https://stackoverflow.com/questions/49512288/no-instance-of-constructor-stdthreadthread-matches-argument-list
		//std::thread temp_thread([this, start_pos, end_pos] {search_substring(start_pos, end_pos); });
		//threads_.emplace_back(temp_thread);

		// SO attempt 1.1 -	tried pointers, it brokey
		//std::thread temp_thread([this, start_pos, end_pos] { this->search_substring(start_pos, end_pos); });
		//std::thread* t = &temp_thread;
		//threads_.emplace_back(t);

		// SO attempt 2 - https://stackoverflow.com/questions/28574004/how-do-i-use-threading-in-a-class
		//std::thread temp_thread(& BoyerMoore::search_substring, this, start_pos, end_pos);

		// SO attempt 3 - https://stackoverflow.com/questions/52004854/start-stdthread-in-member-function-of-object-that-does-not-have-a-copy-constru
		//std::thread temp_thread(&BoyerMoore::search_substring, &start_pos, &end_pos);

		//threads_.emplace_back(temp_thread);
	}

	// join threads
	for (unsigned int i = 0; i < search_thread_count - 1; ++i) threads_[i].join();
}

void BoyerMoore::start_threaded_search()
{
	std::cout << "Boyer Moore threaded search starting" << std::endl;

	// Get the number of available threads on this CPU
	unsigned int num_of_threads = std::thread::hardware_concurrency();
	// num_of_threads = 0 if it cannot get the info
	while (num_of_threads == 0)
	{
		std::cout << "Your CPU count could not be automatically configured!\nPlease enter the number of threads your CPU has: ";
		std::string input;
		std::getline(std::cin, input);
		// try to convert input from string to int
		try
		{
			num_of_threads = static_cast<unsigned int>(std::stoi(input));
		}
		catch (...) {}
	}
	const unsigned int num_of_search_threads = num_of_threads - 1;

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
	const unsigned long long thread_search_width = text.size() / num_of_search_threads;

	std::cout << "\nTotal threads: " << num_of_threads
		<< "\nSearch threads: " << num_of_search_threads
		<< "\nText size:" << text.size()
		<< "\nSearch thread width: " << thread_search_width;

	start_boyer_moore_search_threads(num_of_search_threads);
}

BoyerMoore::BoyerMoore()
{
	type = algorithm_type::boyer_moore;
	for (int i = 0; i < 256; ++i)
		skip_table[i] = pattern.size();

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