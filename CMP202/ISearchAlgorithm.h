#pragma once
#include <string>

class ISearchAlgorithm
{
public:
	virtual std::string get_text();
	virtual std::string get_pattern();
	virtual bool is_threaded();

	virtual ~ISearchAlgorithm();

	virtual void output_search_results() = 0;
	virtual void show_matches() = 0;
	virtual std::string generate_report() = 0;
};