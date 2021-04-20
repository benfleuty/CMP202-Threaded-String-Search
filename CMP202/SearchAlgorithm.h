#pragma once
#include <mutex>
#include <vector>

#include "ISearchAlgorithm.h"
#include <thread>
#include "search_timer.h"

class SearchAlgorithm : public ISearchAlgorithm
{
	std::string pattern_html() const;

protected:
	unsigned long long search_thread_width_ = 0;
	bool search_complete_ = false;
	void replace_matches_in_text();

	unsigned long long match_count_;

public:
	std::string pattern;
	std::string text;
	std::string matched_text;
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
	std::mutex matched_text_mutex;
	std::condition_variable replacer_cv;
	std::condition_variable replacer_finished_cv;
	bool progress_ready = false;
};