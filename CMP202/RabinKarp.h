#pragma once
#include "SearchAlgorithm.h"
class RabinKarp : public SearchAlgorithm
{
private:
	void start_non_threaded_search();
	void search_substring(const unsigned long long& start_pos, const unsigned long long& end_pos);
	void start_rabin_karp_search_threads(const unsigned& search_thread_count);
	void start_threaded_search();

	// Count of possible chars in input
	const unsigned long long alphabet_ = 256;
	// prime number used to calculate hash
	const unsigned long long prime_ = 17;

	long long pattern_hash_val_ = 0;

public:
	RabinKarp();
	unsigned long long calculate_hash_value(unsigned long long& hash_val) const;
	void hash_substring(const std::string& substring, long long& substring_hash_val) const;
	long long get_hash_value(long long pattern_length, long long hash_val, long long alphabet, long long prime);
	long long hash_text(std::string text, long long text_hash, long long pattern_length, long long alphabet,
	                    long long prime);
	long long roll_hash(std::string& text, long long text_hash_val, const long long hash_val, const long long i, const long long pattern_length, const long long alphabet, const long long prime);
	std::vector<long long> search_rabin_karp(std::string& text, std::string& pattern);
	void roll_hash(const std::string& substring, long long& text_hash_val, const unsigned long long& hash_val, const unsigned long long& i) const;
	void search_rabin_karp_non_threaded();
	void start_search() override;
};
