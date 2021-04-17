#pragma once
#include <mutex>
#include <vector>

#include "ISearchAlgorithm.h"
#include <thread>
#include "search_timer.h"

class SearchAlgorithm : public ISearchAlgorithm
{
protected:
	unsigned int total_threads_size_ = 0;
	unsigned int search_threads_size_ = 0;
	unsigned long long search_thread_width_ = 0;

public:
	std::string pattern;
	std::string text;
	std::vector<long long> matching_indexes;
	enum class algorithm_type { not_set, boyer_moore, rabin_karp };
	algorithm_type type;

	search_timer timer;

	bool threaded;

	SearchAlgorithm();

	void output_search_results() override;
	void show_matches() override;
	void store_match_pos(unsigned long long match_pos);
	std::string generate_report() override;

	virtual void start_search() = 0;

	std::mutex matching_indexes_mutex;
	std::condition_variable progress_cv;
	bool progress_ready_ = false;
};