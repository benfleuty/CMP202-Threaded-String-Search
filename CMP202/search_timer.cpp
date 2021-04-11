#include "search_timer.h"

std::chrono::steady_clock::time_point start_time;
std::chrono::steady_clock::time_point end_time;

void search_timer::start()
{
	start_time = std::chrono::steady_clock::now();
}

void search_timer::stop()
{
	end_time = std::chrono::steady_clock::now();
}

long long search_timer::elapsed_time_ms() const
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
}

long long search_timer::elapsed_time_us() const
{
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
}