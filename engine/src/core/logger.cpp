#include "core/logger.h"

#include <regex>

namespace zaphod::log
{

bool validateFormatParameters(const std::string& format, const std::unordered_map<std::string, std::string>& parameters)
{
	std::regex placeholder_regex(R"(%\{([A-Z]*)\}%)");
	auto	   words_begin = std::sregex_iterator(format.begin(), format.end(), placeholder_regex);
	auto	   words_end   = std::sregex_iterator();

	// Uses regex to find all placeholders in the format string
	std::set<std::string> found_placeholders;
	for (std::sregex_iterator i = words_begin; i != words_end; ++i)
		found_placeholders.insert((*i)[1].str());	 // Extract the placeholder name without %{}%

	if (found_placeholders.size() != std::distance(words_begin, words_end))
		return false;	 // Duplicate placeholders found

	bool isMessageInFormat = false;
	for (const auto& placeholder : found_placeholders)
	{
		if (parameters.find(placeholder) == parameters.end())
		{
			if (placeholder == "MESSAGE")
				isMessageInFormat = true;	 // Message placeholder found in format string
			else
				return false;	 // Placeholder not found in parameters
		}
		else if (placeholder == "MESSAGE")
			return false;	 // MESSAGE should not be in parameters
	}
	if (isMessageInFormat)
		return true;	// All placeholders are valid and MESSAGE is present in format string
	else
		return false;	 // MESSAGE placeholder is mandatory
}

//! Set the log format and parameters.
/*!
	Sets the log message format and validates the provided parameters against the format string.
	The format string can contain several unique placeholders in the form of `%{TAG}%`,
	where 'TAG' is a token that contains only uppercase letters, numbers, and underscores.
	The format string must contain the message placeholder, i.e. '%{MESSAGE}%'.
*/
/*!
	The parameters map should contain key-value pairs where keys correspond to the placeholders
	in the format string (excluding the mandatory `MESSAGE` placeholder).
*/
/*!
	@param format - The log message format string.@param parameters -
	A map of parameter tokens to their corresponding values.
*/
void Logger::setFormat(const std::string& format, const std::unordered_map<std::string, std::string> parameters)
{
	validateFormatParameters(format, parameters);

	m_format = format;
	m_parameters.clear();
	m_parameters = parameters;
}

void Logger::updateFormatParameter(const std::string& token, const std::string& value)
{
	if (m_parameters.find(token) != m_parameters.end())
	{
		m_parameters[token] = value;
	}
	else
	{
		// Handle error: token not found
	}
}

void   StandardLoggerBuilder::buildFormat() {}

void   StandardLoggerBuilder::buildDestinations() {}

void   StandardLoggerBuilder::buildLogLevel() {}

Logger StandardLoggerBuilder::construct()
{
	return m_logger;
}

}	 // namespace zaphod::log