#pragma once
#include <mutex>
#include <vector>

#include "SearchAlgorithm.h"
class BoyerMoore : public SearchAlgorithm
{
private:
	void start_non_threaded_search();
	std::vector<long long> non_threaded_search();
	void search_substring(unsigned long long start_pos, unsigned long long end_pos);
	void start_boyer_moore_search_threads(const unsigned int& search_thread_count);

	void start_threaded_search();

	long long skip_table[256] = {};

	std::mutex matching_indexes_mutex;
	std::condition_variable progress_cv;
	bool progress_ready_ = false;

public:
	BoyerMoore();
	void start_search() override;
};
