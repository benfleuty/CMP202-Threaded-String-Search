#define _CRT_SECURE_NO_WARNINGS // allow ctime
#include "SearchAlgorithm.h"

#include<iostream>
#include<string>
#include<ctime>
#include <mutex>
#include <regex>
#include "utils.h"
#include <boost/regex.hpp>

#include "reporter.h"

SearchAlgorithm::SearchAlgorithm()
{
	pattern = "the";// ISearchAlgorithm::get_pattern();
	text = ISearchAlgorithm::get_text();
	threaded = true; // ISearchAlgorithm::is_threaded();
	matched_text = text;
}

void SearchAlgorithm::output_search_results()
{
	std::cout << "Generating report..." << std::endl;

	std::unique_lock<std::mutex> lock(matching_indexes_mutex);
	while(!output_finished){}
	
	std::cout << match_count_ << " matches were found in " << timer.elapsed_time_ms() << "ms (" << timer.elapsed_time_us() << "us)" << std::endl;
	/*
	// see if user wants a file generated
	bool make_report = false;
	do
	{
		std::cout << "\nWould you like to see the matches highlighted in an HTML file? [y/n] ";
		std::string in;
		std::getline(std::cin, in);
		char input = tolower(in[0]);

		if (input == 'y') {
			make_report = true;
			break;
		}
		if (input == 'n') {
			make_report = false;
			break;
		}
	} while (true);

	if (!make_report) return;
	

	std::string report = generate_report();
	reporter::write_report_to_file(report);
	show_matches();
	*/
}

// Thanks to https://gist.github.com/polaris/adee936198995a6f8c697c419d21f734
std::string time_point_as_string(const std::chrono::system_clock::time_point& tp) {
	std::time_t t = std::chrono::system_clock::to_time_t(tp);
	std::string ts = std::ctime(&t);
	ts.resize(ts.size() - 1);
	return ts;
}

//std::string SearchAlgorithm::generate_report()
//{
//	// replace matches with span for highlighting in html
//	//std::string report_text = reporter::process_body_text(text, pattern);
//	//replace_matches_in_text();
//
//	std::string html = R"(<!DOCTYPE html><html lang="en"><head><title><page_title></title><meta charset="utf-8"/><meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no"/><style>body{width:60%;margin:0 auto; font-size:1.2em;}span{background-color: cyan;}</style></head><body><date_time_of_search><br/><matches_found><br/><elapsed_time><br/><searched_text></body></html>)";
//	// add title
//	std::string new_title;
//
//	if (type == algorithm_type::boyer_moore)
//		new_title += "Boyer Moore ";
//	else if (type == algorithm_type::rabin_karp)
//		new_title += "Rabin Karp ";
//
//	new_title += "String Search ";
//
//	if (!threaded)
//		new_title += "Non-";
//	new_title += "Threaded";
//
//	std::string elapsed_time = std::to_string(timer.elapsed_time_ms());
//	elapsed_time += "ms (";
//	elapsed_time += std::to_string(timer.elapsed_time_us());
//	elapsed_time += "us)";
//
//	// Get the current time to record on the search.
//	std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();
//	// to string
//	std::string now = time_point_as_string(now_tp);
//	// remove colons from the datetime
//	std::string delim = ".";
//	std::string file_friendly_time = reporter::regex_text_replacer(now, ":", delim);
//	reporter::report_name = new_title + " " + file_friendly_time + ".html";
//	std::string doc_time = "Search Date & Time: " + now;
//
//	// matching_indexes.size() to string
//	std::string match_count = std::to_string(match_count_) + " matches";
//	//std::string match_count = std::to_string(matching_indexes.size()) + " matches";
//
//	// replace <title>
//	html = reporter::regex_text_replacer(html, "<page_title>", new_title);
//	// replace <date_time_of_search>
//	html = reporter::regex_text_replacer(html, "<date_time_of_search>", doc_time);
//	// replace <matches_found>
//	html = reporter::regex_text_replacer(html, "<matches_found>", match_count);
//	// replace <elapsed_time>
//	html = reporter::regex_text_replacer(html, "<elapsed_time>", elapsed_time);
//	// replace <searched_text> with highlighted text
//	//html = reporter::regex_text_replacer(html, "<searched_text>", report_text);
//	html = reporter::regex_text_replacer(html, "<searched_text>", matched_text);
//
//	return html;
//}

//void SearchAlgorithm::show_matches()
//{
//	std::string cmd = "start \"\" \"" + reporter::report_name + "\"";
//	system(cmd.c_str());
//}

void SearchAlgorithm::store_match_pos(unsigned long long match_pos)
{
	if (threaded) {
		std::unique_lock<std::mutex> lock(matching_indexes_mutex);
		matching_indexes.emplace_back(match_pos);
		lock.unlock();
		progress_ready = true;
		output_cv.notify_one();
		return;
	}
	matching_indexes.emplace_back(match_pos);
}


void SearchAlgorithm::output_match()
{
	std::unique_lock<std::mutex> lock(matching_indexes_mutex);

	while(true)
	{
		while (!progress_ready && !search_complete_)
			output_cv.wait(lock);

		progress_ready = false;
		if (matching_indexes.empty()) {
			if (search_complete_) break;
			continue;
		}
		size_t pos = matching_indexes.back();
		matching_indexes.pop_back();
		if(!benchmarked)
			show_context(text, pos);
		match_count_++;
	}

	output_finished = true;
	
}


//void SearchAlgorithm::replace_matches_in_text()
//{
//	matched_text = text;
//	std::unique_lock<std::mutex> matching_indexes_mutex_lock(matching_indexes_mutex);
//	std::string rgx = R"((?<!>)()" + pattern + ")";
//	const boost::regex regex_pattern{ rgx };
//
//	int x = 0;
//	
//	while (true)
//	{
//		while (!progress_ready && !search_complete_)
//			output_cv.wait(matching_indexes_mutex_lock);
//
//		if (search_complete_ && matching_indexes.empty()) break;
//		if (matching_indexes.empty()) continue;
//
//		std::cout << "replacer starting" << std::endl;
//
//		progress_ready = false;
//
//		std::unique_lock<std::mutex> matched_text_mutex_lock(matched_text_mutex);
//		matching_indexes.pop_back();
//		
//		matched_text = boost::regex_replace(matched_text, regex_pattern, pattern_html(),boost::format_first_only);
//		
//		match_count_++;
//		std::cout << "Replaced match at text[" << pos_keep << "]" << std::endl;
//		std::cout << "item replaced" << std::endl;
//		std::cout << x << std::endl;
//		if (++x >= 100) break;
//	}
//	std::cout << "replacer finished\n";
//	replacer_finished_cv.notify_one();
//}

//void SearchAlgorithm::replace_matches_in_text()
//{
//	//std::unique_lock<std::mutex> matching_indexes_mutex_lock(matching_indexes_mutex);
//	//unsigned long long pos = matching_indexes.back();
//	//matching_indexes.pop_back();
//	//matching_indexes_mutex_lock.release();
//	
//	//show_context(text, pos);
//
//	const size_t replacer_skip_value = pattern_html().size() - pattern.size();
//	
//	std::unique_lock<std::mutex> matching_indexes_mutex_lock(matching_indexes_mutex);
//
//	while (true)
//	{
//		while (!progress_ready && !search_complete_)
//			output_cv.wait(matching_indexes_mutex_lock);
//
//		if (search_complete_ && matching_indexes.empty()) break;
//		if (matching_indexes.empty()) continue;
//
//		//std::cout << "replacer starting" << std::endl;
//
//		progress_ready = false;
//
//		unsigned long long pos = matching_indexes.back();
//		matching_indexes.pop_back();
//
//		matching_indexes_mutex_lock.unlock();
//
//	//	/*-----------------//
//
//	//	match_count_++;
//	//	std::cout << "replacer done\n" << match_count_ << std::endl;
//	//	continue;
//
//	//	//-----------------*/
//
//		std::string substring = matched_text.substr(pos, pattern.size());
//
//	//	/*
//	//	 * a: substring != pattern
//	//	 * b: matched_text[pos - 1] != '>'
//	//	 *
//	//	 * Logic: ¬a + ¬a.b
//	//	*/
//	//	/*
//	//	while (true)
//	//	{
//	//		// if not the pattern
//	//		if(substring != pattern)
//	//		{
//	//			pos++;
//	//		}
//
//	//		if(matched_text[pos-1] == '>')
//	//		{
//	//			pos++;
//	//		}
//	//	}
//	//	*/
//
//		while (substring != pattern || matched_text[pos - 1] == '>')
//		{
//			//if (matched_text[pos - 1] == '>')
//			if(substring != pattern)
//			{
//				if (pos + replacer_skip_value + pattern.size() >= matched_text.size()) break;
//				pos += replacer_skip_value;
//			}
//			//else if (pos + 1 < matched_text.size())
//			//	pos++;
//			substring = matched_text.substr(pos, pattern.size());
//			/*
//			if (pos + 1 + pattern.size() < matched_text.size())
//				pos += 1 + pattern.size();
//
//			else if (pos + 1 + pattern.size() >= matched_text.size()) break;
//
//			substring = matched_text.substr(pos, pattern.size());
//			*/
//		}
//
//		if (pos + pattern.size() >= matched_text.size()) break;
//
//		std::string left = matched_text.substr(0, pos);
//		std::string right = matched_text.substr(pos + pattern.size(), matched_text.size());
//
//		matched_text = left;
//		matched_text += pattern_html();
//		matched_text += right;
//
//		match_count_++;
//		//std::cout << "Replaced match at text[" << pos_keep << "]" << std::endl;
//		//std::cout << "item replaced" << std::endl;
//	}
//	//std::cout << "replacer finished\n";
//	//replacer_finished_cv.notify_one();
//}

//unsigned long long xxx;

//void SearchAlgorithm::replace_matches_in_text(unsigned long long start_pos, unsigned long long end_pos)
//{
//	/*
//	while(!matching_indexes.empty() || matching_indexes.empty() && search_complete_ == false )
//	{
//		std::unique_lock<std::mutex> matching_indexes_mutex_lock(matching_indexes_mutex);
//		if (!matching_indexes.empty())
//		{
//			unsigned long long pos = matching_indexes.back();
//			matching_indexes.pop_back();
//			match_count_++;
//			matching_indexes_mutex_lock.unlock();
//			show_context(text, pos);
//			std::cout << match_count_ << "\n";
//		}
//	}
//	return;
//	matched_text = text;
//	*/
//
//	std::string local_text = text.substr(start_pos, end_pos - start_pos);
//	unsigned long long replacer_skip_value = pattern_html().size() - pattern.size();
//
//	
//	while (true)
//	{
//		bool found = false;
//		std::unique_lock<std::mutex> matching_indexes_mutex_lock(matching_indexes_mutex);
//
//		while (!progress_ready && !search_complete_)
//			output_cv.wait(matching_indexes_mutex_lock);
//
//		if (search_complete_ && matching_indexes.empty()) break;
//		if (matching_indexes.empty()) continue;
//
//		progress_ready = false;
//
//		unsigned long long pos;
//
//		for (size_t i = 0; i < matching_indexes.size();++i)
//		{
//			auto x = matching_indexes[i];
//			if (x >= start_pos && x < end_pos)
//			{
//				pos = x;
//				std::cout << "erase\n";
//				matching_indexes.erase(matching_indexes.begin() + i);
//				std::cout << "unlock\n";
//				matching_indexes_mutex_lock.unlock();
//				found = true;
//				break;
//			}
//		}
//
//		if(!found) continue;
//		
//		xxx++;
//		matching_indexes.pop_back();
//		matching_indexes_mutex_lock.unlock();
//
//		pos -= start_pos;
//		
//
//		std::string substring = local_text.substr(pos, pattern.size());
//
//		while (substring != pattern || substring == pattern && local_text[pos - 1] == '>')
//		{
//			if (pos + replacer_skip_value + pattern.size() >= local_text.size()) break;
//			pos += replacer_skip_value;
//			substring = local_text.substr(pos, pattern.size());
//		}
//
//		if (pos + replacer_skip_value + pattern.size() >= local_text.size()) break;
//
//		std::string left = local_text.substr(0, pos);
//		std::string right = local_text.substr(pos + pattern.size(), local_text.size());
//
//		local_text = left;
//		local_text += pattern_html();  
//		local_text += right;
//
//		match_count_++;
//		std::cout << "replaced\n";
//	}
//	text_fragment tf{ local_text,start_pos };
//	std::unique_lock<std::mutex> local_text_fragments_mutex_lock(local_text_fragments_mutex_);
//	local_text_fragments_.emplace_back(tf);
//	replacer_finished_cv.notify_one();
//	std::cout << "replacer exit\n";
//}

//std::string SearchAlgorithm::pattern_html() const
//{
//	return R"(<span>)" + pattern + R"(</span>)";
//	//return R"(<span class="match">)" + pattern + R"(</span>)";
//}