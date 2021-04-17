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
	long long get_hash_value(long long& hash_val) const;
	void hash_substring(const std::string& substring, long long& substring_hash_val) const;
	void roll_hash(const std::string& substring, long long& text_hash_val, const long long& hash_val, const unsigned long long& i) const;
	void search_rabin_karp_non_threaded();
	void start_search() override;
};
