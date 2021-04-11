#define _CRT_SECURE_NO_WARNINGS // allow ctime
#include "SearchAlgorithm.h"

#include<iostream>
#include<string>
#include<ctime>
#include "reporter.h"

SearchAlgorithm::SearchAlgorithm()
{
	pattern = ISearchAlgorithm::get_pattern();
	text = ISearchAlgorithm::get_text();
	threaded = ISearchAlgorithm::is_threaded();
}

void SearchAlgorithm::output_search_results()
{
	std::cout << matching_indexes.size() << " matches were found in " << timer.elapsed_time_ms() << "ms" << std::endl;
	std::cout << "Generating report..." << std::endl;
	std::string report = generate_report();
	reporter::write_report_to_file(report);
	show_matches();
}

// Thanks to https://gist.github.com/polaris/adee936198995a6f8c697c419d21f734
std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
	std::time_t t = std::chrono::system_clock::to_time_t(tp);
	std::string ts = std::ctime(&t);
	ts.resize(ts.size() - 1);
	return ts;
}

std::string SearchAlgorithm::generate_report()
{
	// replace matches with span for highlighting in html
	std::string report_text = reporter::process_body_text(text, pattern);

	std::string html = R"(<!DOCTYPE html><html lang="en"><head><title><page_title></title><meta charset="utf-8"/><meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no"/><style>body{width:60%;margin:0 auto; font-size:1.2em;}.match{background-color: cyan;}</style></head><body><date_time_of_search><br/><elapsed_time><br/><searched_text></body></html>)";
	// add title
	std::string new_title = "unspecified";
	if (type == algorithm_type::boyer_moore)
		new_title = "Boyer Moore";
	else if (type == algorithm_type::rabin_karp)
		new_title = "Rabin Karp";

	new_title += " String Search";

	std::string elapsed_time = std::to_string(timer.elapsed_time_ms());
	elapsed_time += "ms (";
	elapsed_time += std::to_string(timer.elapsed_time_us());
	elapsed_time += "us)";

	// Get the current time to record on the search.
	std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();
	// to string
	std::string now = timePointAsString(now_tp);
	// remove colons from the datetime
	std::string delim = ".";
	std::string file_friendly_time = reporter::regex_text_replacer(now, ":", delim);
	reporter::report_name = new_title + " " + file_friendly_time + ".html";
	std::string doc_time = "Search Date & Time: " + now;

	// replace <title>
	html = reporter::regex_text_replacer(html, "<page_title>", new_title);
	// replace <date_time_of_search>
	html = reporter::regex_text_replacer(html, "<date_time_of_search>", doc_time);
	// replace <elapsed_time>
	html = reporter::regex_text_replacer(html, "<elapsed_time>", elapsed_time);
	// replace <searched_text> with highlighted text
	html = reporter::regex_text_replacer(html, "<searched_text>", report_text);

	return html;
}

void SearchAlgorithm::show_matches()
{
	std::string cmd = "start \"\" \"" + reporter::report_name + "\"";
	system(cmd.c_str());
}