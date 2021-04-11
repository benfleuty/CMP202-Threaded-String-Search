#include "reporter.h"

#include <fstream>
#include <iostream>
#include <regex>

std::string reporter::report_name;

reporter::~reporter() = default;

void reporter::write_report_to_file(std::string & html)
{
	std::ofstream output(report_name);
	output << html;
	output.close();
}

std::string reporter::regex_text_replacer(const std::string & text, const std::string & pattern, std::string & new_value) {
	const std::regex regex_pattern(pattern);
	return std::regex_replace(text, regex_pattern, new_value);
}

std::string reporter::process_body_text(const std::string & text, const std::string & pattern)
{
	const std::string new_value = R"(<span class="match">)" + pattern + R"(</span>)";
	//const std::string temp = text;
	const std::regex regex_pattern(pattern);
	return std::regex_replace(text, regex_pattern, new_value);
	//return regex_text_replacer(text, regex_pattern, new_val.c_str());
}