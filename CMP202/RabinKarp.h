#pragma once
#include "SearchAlgorithm.h"
class RabinKarp : public SearchAlgorithm
{
private:
	// self



	// shared
	// count of possible chars in input
	const long long alphabet_ = 256;
	// prime number used to calculate hash
	const long long prime_ = 17;
	long long pattern_hash_val_ = 0;
	long long hash_val_ = 1;
	
	void calc_hash_value(const unsigned long long& hash_val_seed);
	void hash_text(long long& string_hash);
	void hash_pattern();
	void roll_hash(long long& text_hash_val, const unsigned long long i);

	// non threaded
	void start_non_threaded_search();
	void search_rabin_karp_non_threaded();

	// threaded
	void start_threaded_search();
	void start_rabin_karp_search_threads(const unsigned& search_thread_count);
	void search_substring(const unsigned long long& start_pos, const unsigned long long& end_pos);
	void hash_substring(const std::string& substring, long long& substring_hash_val) const;

public:
	// self
	RabinKarp();
	void start_search() override;
};
