// ReSharper disable CppUseAuto
#include "RabinKarp.h"

#include <iostream>

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
	for (char i : pattern)
		pattern_hash_val_ = (alphabet_ * pattern_hash_val_ + i) % prime_;
}

void RabinKarp::start_search()
{
	start_threaded_search();
	/*
	if (threaded)
		start_threaded_search();
	else start_non_threaded_search();
	*/
}

// shared

void RabinKarp::hash_text(long long& string_hash) {
	for (unsigned long long i = 0; i < pattern.size(); ++i)
		string_hash = (alphabet_ * string_hash + text[i]) % prime_;
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
		text_hash_val += 17;
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
				if (text[i+j] != pattern[j]) break;

			//  j == patternLength when the two matching hashes have been compared char for char
			// add the index to the list of matches
			if (j == pattern.size())
				store_match_pos(i);
		}

		// if i is in range roll the hash to the next check
		if (i < text.size() - pattern.size())
			roll_hash( text_hash_val, i);
	}
}

// threaded

