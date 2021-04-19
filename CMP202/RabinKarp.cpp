#include "RabinKarp.h"

#include <iostream>

RabinKarp::RabinKarp()
{
	type = algorithm_type::rabin_karp;
	//pattern_hash_val_ = hash_substring(pattern, pattern_hash_val_);
}

unsigned long long RabinKarp::calculate_hash_value(unsigned long long& hash_val) const
{
	for (unsigned long long i = 0; i < pattern.size() - 1; ++i)
		hash_val = (hash_val * alphabet_) % prime_;
	return hash_val;
}

void RabinKarp::hash_substring(const std::string& substring, long long& substring_hash_val) const
{
	for (unsigned int i = 0; i < pattern.size(); ++i)
		substring_hash_val = (alphabet_ * substring_hash_val + substring[i]) % prime_;
	//return substring_hash_val;
}

void RabinKarp::roll_hash(const std::string& substring, long long& text_hash_val, const unsigned long long& hash_val, const unsigned long long& i) const
{
	/* Get hash value of the next position
	 * Subtract hash value of substring[i]
	 * Add value of substring[i + pat_len]
	 * Divide total by prime number
	 */
	const auto pos = i + pattern.size();
	text_hash_val = (alphabet_ * (text_hash_val - substring[i] * hash_val) + substring[pos]) % prime_;

	// if the text hash is below 0
	// add prime number to substring hash

	if (text_hash_val < 0)
		text_hash_val += prime_;

	//return text_hash_val;
}


long long get_hash_value(const long long pattern_length, long long hash_val, const long long alphabet, const long long prime) {
	for (long long i = 0; i < pattern_length - 1; ++i)
		hash_val = (hash_val * alphabet) % prime;
	return hash_val;
}

long long hash_text(std::string text, long long text_hash, const long long pattern_length, const long long alphabet, const long long prime) {
	for (long long i = 0; i < pattern_length; ++i)
		text_hash = (alphabet * text_hash + text[i]) % prime;
	return text_hash;
}

long long roll_hash(std::string& text, long long text_hash_val, const long long hash_val, const long long i, const long long pattern_length, const long long alphabet, const long long prime) {
	/* Get hash value of the next position
	 * Subtract hash value of text[i]
	 * Add value of text[i + patlen]
	 * Divide total by prime number
	 */

	long long pos = i + pattern_length;
	text_hash_val = (alphabet * (text_hash_val - text[i] * hash_val) + text[pos]) % prime;

	// if textHash is below 0
	// add prime number to text hash

	if (text_hash_val < 0)
		text_hash_val += prime;

	return text_hash_val;
}

std::vector<long long> search_rabin_karp(std::string& text, std::string& pattern) {
	// vector to hold returnable data
	std::vector<long long> matching_indexes;
	// Get lengths
	const long long pattern_length = pattern.size();
	const long long text_length = text.size();

	// Count of possible chars in input
	const long long alphabet = 256;
	// Hash value of the pattern
	long long pattern_hash_val = 0;
	// Hash value of the text
	long long text_hash_val = 0;
	// prime number used to calculate hash
	const long long prime = 17;

	// Calculate the hash value
	// initialise
	long long hash_val = 1;
	// get hash value
	hash_val = get_hash_value(pattern_length, hash_val, alphabet, prime);

	// Keep iterators in scope
	long long i = 0, j = 0;

	// Get hash values of the pattern and text
	pattern_hash_val = hash_text(pattern, pattern_hash_val, pattern.size(), alphabet, prime);
	text_hash_val = hash_text(text, text_hash_val, pattern.size(), alphabet, prime);

	// iterate 0 through textlength - pattern length (last possible pos)
	for (i = 0; i <= text_length - pattern_length; ++i) {
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val == text_hash_val) {
			// iterate each char for the length of the potential match
			for (j = 0; j < pattern_length; j++) {
				// check each char

				const auto pos = i + j;
				if (text[pos] != pattern[j])
					// break if mismatch
					break;
			}

			//  j == patternLength when the two matching hashes have been compared char for char
			if (j == pattern_length)
				// add the index to the list of matches
				matching_indexes.push_back(i);
		}

		// if i is in range
		if (i < text_length - pattern_length)
			// roll the hash to the next check
			text_hash_val = roll_hash(text, text_hash_val, hash_val, i, pattern.size(), alphabet, prime);
	}
	return matching_indexes;
}





void RabinKarp::search_rabin_karp_non_threaded() {
	// Hash value of the pattern
	long long pattern_hash_val = 0;
	// Hash value of the text
	long long text_hash_val = 0;

	// Calculate the hash value
	// initialise
	unsigned long long hash_val = 1;
	// get hash value
	hash_val = calculate_hash_value(hash_val);

	// Keep iterators in scope
	unsigned long long i = 0, j = 0;

	// Get hash values of the pattern and text
	hash_substring(pattern, pattern_hash_val);
	hash_substring(text, text_hash_val);

	// iterate 0 through text_length - pattern.size() (last possible pos)
	for (i = 0; i <= text.size() - pattern.size(); ++i) {
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val == text_hash_val) {
			// iterate each char for the length of the potential match
			for (j = 0; j < pattern.size(); j++) {
				// check each char

				const auto pos = i + j;
				if (text[pos] != pattern[j])
					// break if mismatch
					break;
			}

			//  j == patternLength when the two matching hashes have been compared char for char
			if (j == pattern.size())
				// add the index to the list of matches
				store_match_pos(i);
		}

		// if i is in range
		if (i < text.size() - pattern.size())
			// roll the hash to the next check
			roll_hash(text, text_hash_val, hash_val, i);
	}
}

void RabinKarp::start_non_threaded_search()
{
	std::cout << "Rabin Karp non threaded search starting\n";
	timer.start();
	//search_rabin_karp_non_threaded();
	matching_indexes = search_rabin_karp(text,pattern);
	timer.stop();
}

void RabinKarp::search_substring(const unsigned long long& start_pos, const unsigned long long& end_pos)
{
	// Hash value of the pattern
	long long pattern_hash_val = 0;
	// Hash value of the text
	long long text_hash_val = 0;

	// Calculate the hash value
	// initialise
	unsigned long long hash_val = 1;
	// get hash value
	hash_val = calculate_hash_value(hash_val);

	// Keep iterators in scope
	unsigned long long j = 0;

	// Get hash values of the pattern and text
	hash_substring(pattern, pattern_hash_val);
	hash_substring(text, text_hash_val);

	// iterate 0 through text_length - pattern.size() (last possible pos)
	for (auto i = start_pos; i <= end_pos - pattern.size(); ++i) {
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val == text_hash_val) {
			// iterate each char for the length of the potential match
			for (j = 0; j < pattern.size(); j++) {
				// check each char

				const auto pos = i + j;
				if (text[pos] != pattern[j])
					// break if mismatch
					break;
			}

			//  j == patternLength when the two matching hashes have been compared char for char
			if (j == pattern.size())
				// add the index to the list of matches
				store_match_pos(i);
		}

		// if i is in range
		if (i < end_pos - pattern.size())
			// roll the hash to the next check
			roll_hash(text, text_hash_val, hash_val, i);
	}
}
/*
void RabinKarp::search_substring(unsigned long long start_pos, unsigned long long end_pos)
{
	// Hash value of the substring
	long long substring_hash_val = 0;

	// Calculate the hash value
	// initialise to 1
	const long long hash_val = calculate_hash_value(1);

	// keep j in scope
	unsigned long long j;

	// Get hash values of the pattern and text
	substring_hash_val = hash_substring(text, substring_hash_val);

	// iterate 0 through text_length - pattern.size() (last possible pos)
	for (auto i = start_pos; i <= end_pos; ++i) {
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val_ == substring_hash_val) {
			// iterate each char for the length of the potential match
			for (j = 0; j < pattern.size(); j++) {
				// check each char
				const long long pos = i + j;
				if (text[pos] != pattern[j])
					// break if mismatch
					break;
			}

			if (j == pattern.size())
				// add the index to the list of matches
				store_match_pos(i);
		}

		// if i is in range
		if (i < text.size() - pattern.size())
			// roll the hash to the next check
			substring_hash_val = roll_hash(text, substring_hash_val, hash_val, i);
	}
}
*/
void RabinKarp::start_rabin_karp_search_threads(const unsigned int& search_thread_count)
{
	unsigned long long width = search_thread_width_;
	unsigned long long start_pos = 0;
	unsigned long long end_pos = 0;
	// start search threads X1 to Xn-2
	std::vector<std::thread> threads;
	for (unsigned int i = 1; i < search_thread_count; ++i)
	{
		// start_pos is the value of the search width multiplied by the number of threads that have already been started (i-1)
		start_pos = search_thread_width_ * (i - 1);
		// end_pos remains one tick ahead of start pos, leaving a width of search_thread_width_ between the start and end
		end_pos = search_thread_width_ * i;

		threads.emplace_back(std::thread(&RabinKarp::search_substring, this, start_pos, end_pos));
	}

	// start search thread X-1
	start_pos = end_pos;
	end_pos = text.size();
	threads.emplace_back(std::thread(&RabinKarp::search_substring, this, start_pos, end_pos));

	// join all threads
	for (auto& thread : threads) thread.join();
}

void RabinKarp::start_threaded_search()
{
	std::cout << "Rabin Karp threaded search starting\n";

	timer.start();

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

	std::cout << "\nTotal threads: " << num_of_threads
		<< "\nSearch threads: " << num_of_search_threads
		<< "\nText size:" << text.size()
		<< "\nSearch thread width: " << search_thread_width_ << std::endl;

	start_rabin_karp_search_threads(num_of_search_threads);
	timer.stop();
}

void RabinKarp::start_search()
{
	if (threaded)
		start_threaded_search();
	else start_non_threaded_search();
}