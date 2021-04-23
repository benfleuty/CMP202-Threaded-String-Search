// ReSharper disable CppUseAuto
#include "RabinKarp.h"

#include <iostream>

#include "utils.h"

// self

RabinKarp::RabinKarp()
{
	type = algorithm_type::rabin_karp;
	calc_hash_value(1);
	hash_pattern();
}

void RabinKarp::calc_hash_value(const unsigned long long& hash_val_seed)
{
	hash_val_ = hash_val_seed;
	for (unsigned long long i = 0; i < pattern.size() - 1; ++i)
		hash_val_ = (hash_val_ * alphabet_) % prime_;
}

void RabinKarp::hash_pattern() {
	pattern_hash_val_ = 0;
	for (char i : pattern)
		pattern_hash_val_ = (alphabet_ * pattern_hash_val_ + i) % prime_;
}

void RabinKarp::start_search()
{
	if (threaded)
		start_threaded_search();
	else start_non_threaded_search();
}

// shared

void RabinKarp::hash_text(long long& string_hash, const unsigned long long start = 0) {
	for (unsigned long long i = 0; i < pattern.size(); ++i)
		string_hash = (alphabet_ * string_hash + text[i + start]) % prime_;
}

void RabinKarp::roll_hash(long long& text_hash_val, const unsigned long long i) {
	/* Get hash value of the next position
	 * Subtract hash value of text[i]
	 * Add value of text[i + patlen]
	 * Divide total by prime number
	 */

	unsigned long long pos = i + pattern.size();
	text_hash_val = (alphabet_ * (text_hash_val - text[i] * 1) + text[pos]) % prime_;

	if (text_hash_val < 0)
		text_hash_val += prime_;
}

// non threaded

void RabinKarp::start_non_threaded_search()
{
	std::cout << "Rabin Karp non threaded search starting\n";
	timer.start();
	//search_rabin_karp_non_threaded();
	search_rabin_karp_non_threaded();
	timer.stop();
}

void RabinKarp::search_rabin_karp_non_threaded() {
	// Hash value of the text
	long long text_hash_val = 0;
	// Keep iterators in scope
	unsigned long long i = 0;
	unsigned long long j = 0;

	// Get hash values of the pattern and text
	hash_text(text_hash_val);

	// iterate 0 through textlength - pattern length (last possible pos)
	for (i = 0; i <= text.size() - pattern.size(); ++i) {
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val_ == text_hash_val) {
			// iterate each char for the length of the potential match
			// break if a char pos doesn't match
			for (j = 0; j < pattern.size(); j++)
				if (text[i + j] != pattern[j]) break;

			//  j == patternLength when the two matching hashes have been compared char for char
			// add the index to the list of matches
			if (j == pattern.size())
				store_match_pos(i);
		}

		// if i is in range roll the hash to the next check
		if (i < text.size() - pattern.size())
			roll_hash(text_hash_val, i);
	}
}

// threaded

void RabinKarp::search_substring(const unsigned long long& start_pos, const unsigned long long& end_pos)
{
	// Hash value of the text
	long long text_hash_val = 0;
	// Keep iterator j in scope
	unsigned long long j = 0;

	// Get hash values of the pattern and text
	hash_text(text_hash_val, start_pos);

	for (unsigned long long i = start_pos; i < end_pos; ++i) {
		//if (i == 27) std::cout << "Start pos clash: " << start_pos << std::endl;
		// if the pattern's hash is the same as the text's hash
		// then it's likely a match
		if (pattern_hash_val_ == text_hash_val) {
			// iterate each char for the length of the potential match
			// break if a char pos doesn't match
			for (j = 0; j < pattern.size(); j++)
				if (text[i + j] != pattern[j]) break;

			//  j == patternLength when the two matching hashes have been compared char for char
			// add the index to the list of matches
			if (j == pattern.size()) {
				store_match_pos(i);
			}
		}

		// if i is in range roll the hash to the next check
		if (i < text.size() - pattern.size())
			roll_hash(text_hash_val, i);
		//std::cout << "RK search iteration done\n";
	}
}

void RabinKarp::start_search_threads(const unsigned int& search_thread_count)
{
	unsigned long long start_pos = 0;
	unsigned long long end_pos = 0;
	// start search threads X1 to Xn-2
	std::vector<std::thread> search_threads;

	std::thread replacer_thread(&RabinKarp::output_match, this);

	for (size_t i = 1; i < search_thread_count; ++i)
	{
		// start_pos is the value of the search width multiplied by the number of threads that have already been started (i-1)
		start_pos = search_thread_width_ * (i - 1);
		// end_pos remains one tick ahead of start pos, leaving a width of search_thread_width_ between the start and end
		end_pos = search_thread_width_ * i;
	
		search_threads.emplace_back(std::thread(&RabinKarp::search_substring, this, start_pos, end_pos));
	}

	// start search thread X-1
	start_pos = end_pos;
	end_pos = text.size();
	search_threads.emplace_back(std::thread(&RabinKarp::search_substring, this, start_pos, end_pos));

	// join all search threads
	for (auto& thread : search_threads)
		thread.join();
	
	timer.stop();
	search_complete_ = true;
	output_cv.notify_all();

	replacer_thread.join();
	//std::cout << "Joined replacer thread\n";

//	for (auto& thread : replacer_threads)
//{
//		thread.join();
		//std::cout << "Joined replacer thread\n";
//	}
}

void RabinKarp::start_threaded_search()
{
	std::cout << "Rabin Karp threaded search starting\n";

	timer.start();

	// Get the number of available threads on this CPU
	unsigned int num_of_threads = std::thread::hardware_concurrency();
	// num_of_threads = 0 if it cannot get the info
	while (num_of_threads < 1)
	{
		std::cout << "Your CPU count could not be automatically configured!\nPlease enter the number of threads your CPU can run concurrently: ";
		std::string input;
		std::getline(std::cin, input);
		std::cout << std::endl;
		// try to convert input from string to int
		try
		{
			num_of_threads = static_cast<unsigned int>(std::stoi(input));
		}
		catch (...) {}
	}

	const unsigned int num_of_search_threads = num_of_threads;

	// take the total length of the text and divide it by the number of available cores
	// the last search thread will take up the rest of the characters
	// e.g. 1999 chars spread over 15 threads = 133 positions on threads 1-14 and 138 on thread 15
	search_thread_width_ = text.size() / num_of_search_threads;

	if (!benchmarked) {
		std::cout << "\nConcurrent Threads: " << num_of_threads
			<< "\nSearch threads: " << num_of_search_threads
			<< "\nText size: " << text.size()
			<< "\nSearch for: " << pattern
			<< "\nSearch thread width: " << search_thread_width_ << std::endl;
	}
	start_search_threads(num_of_search_threads);
}


// Benchmarking

void RabinKarp::benchmark()
{
	benchmarked = true;
	std::string original_text = get_text();
	std::string output = "sample size,pattern size, iteration, time taken\n";
	std::ofstream f;
	f.open("benchmark-results-rabin-karp.csv", std::ios_base::binary);
	f << output;
	f.close();

	for (size_t full_loop = 1; full_loop <= 8192; full_loop *= 2)
	{
		switch (full_loop)
		{
		case 32:
		case 512:
		case 4096:
		case 8192:
			break;
		default:
			continue;
		}
		text = original_text;
		// countdown iterator for loop reading
		size_t j = full_loop;
		// if more than 1 set is to be loaded
		// loop until 1 is reached
		while (j > 1) {
			// append self
			text.append(text);
			j /= 2;
		}

		std::vector<std::string> patterns { "the","fried","chicken","Teletubbies" };
		for (size_t pattern_loop = 0; pattern_loop < patterns.size(); pattern_loop++)
		{
			pattern = patterns[pattern_loop];
			hash_pattern();
			for (size_t algo_loop = 0; algo_loop < 100; algo_loop++) {
				match_count_ = 0;
				start_threaded_search();
				
				std::cout << "Sample Size: " << full_loop << " | Pattern: " << pattern << " | Iteration: " << algo_loop << " | Time taken: " << timer.elapsed_time_us() << " | Found: " << match_count_ << std::endl;
				output += std::to_string(full_loop) + "," + std::to_string(patterns[pattern_loop].size()) + "," + std::to_string(algo_loop) + "," + std::to_string(timer.elapsed_time_us()) + "\n";
				
			}
			output += "\n";
		}
	}

	f.open("benchmark-results-rabin-karp.csv", std::ios_base::binary);
	f << output;
	f.close();
}