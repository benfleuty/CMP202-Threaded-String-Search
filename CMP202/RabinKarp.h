#pragma once
#include "SearchAlgorithm.h"
class RabinKarp : public SearchAlgorithm
{
private:
	void start_non_threaded_search();
	void start_threaded_search();
public:
	RabinKarp();
	void start_search() override;
};
