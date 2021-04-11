#pragma once
#include <chrono>

//#define TIME_NOW std::chrono::steady_clock::now()

class search_timer
{
public:
	void start();
	void stop();
	long long elapsed_time_ms() const;
	long long elapsed_time_us() const;
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point end_time;
};
