#pragma once
#include <mutex>
#include <vector>
#include <fstream>

#include "ISearchAlgorithm.h"
#include <thread>
#include "search_timer.h"

class SearchAlgorithm : public ISearchAlgorithm
{
	//std::string pattern_html() const;

	//struct text_fragment
	//{
	//	std::string text;
	//	unsigned long long start_pos;
	//};

	//struct size_sorter
	//{
	//	bool operator()(const text_fragment& x, const text_fragment& y) const
	//	{
	//		return x.start_pos < y.start_pos;
	//	}
	//};

	
	//std::mutex local_text_fragments_mutex_;
	//std::vector<text_fragment> local_text_fragments_;


	bool output_finished = false;

protected:
	unsigned long long search_thread_width_ = 0;
	bool search_complete_ = false;
	//void replace_matches_in_text(unsigned long long start_pos, unsigned long long end_pos);

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
	//void show_matches() override;
	void store_match_pos(unsigned long long match_pos);
	void output_match();
	//std::string generate_report() override;

	virtual void start_search() = 0;

	std::mutex matching_indexes_mutex;
	std::mutex matched_text_mutex;
	std::condition_variable output_cv;
	std::condition_variable output_finished_cv;
	bool progress_ready = false;
	bool benchmarked = false;
};