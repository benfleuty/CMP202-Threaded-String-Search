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

// adapted from https://stackoverflow.com/a/40195721
std::string reporter::sanitise_input(const std::string & pattern)
{
	const std::regex regex_special_chars{ R"([-[\]{}()*+?.,\^$|#\s])" };
	return std::regex_replace(pattern, regex_special_chars, R"(\$&)");
}

std::string reporter::regex_text_replacer(const std::string & text, const std::string & pattern, std::string & new_value) {
	const std::string sanitized = sanitise_input(pattern);
	const std::regex regex_pattern(sanitized);
	return std::regex_replace(text, regex_pattern, new_value);
}

std::string reporter::process_body_text(const std::string & text, const std::string & pattern)
{
	const std::string sanitized = sanitise_input(pattern);
	const std::string new_value = R"(<span class="match">)" + sanitized + R"(</span>)";
	const std::regex regex_pattern(sanitized);
	return std::regex_replace(text, regex_pattern, new_value);
}