#pragma once
#include <string>

class reporter
{
public:
	static std::string report_name;
	static void write_report_to_file(std::string& html);
	static std::string regex_text_replacer(const std::string& text, const std::string& pattern, std::string& new_value);

	static std::string process_body_text(const std::string& text, const std::string& pattern);

	static std::string sanitise_input(const std::string& pattern);

	~reporter();
};
