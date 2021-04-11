#pragma once
#include <vector>

#include "ISearchAlgorithm.h"
#include <thread>
#include "search_timer.h"

class SearchAlgorithm : public ISearchAlgorithm
{
protected:
	std::vector<std::thread> threads_;
	unsigned int total_threads_size_;
	unsigned int search_threads_size_;
	unsigned long long search_thread_width_;

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
	std::string generate_report() override;

	virtual void start_search() = 0;
};