#include "core/logger.h"

#include <regex>
#include <set>

namespace zaphod
{

bool validateFormatParameters(const std::string& format, const std::unordered_map<std::string, std::string>& parameters)
{
	std::regex			  placeholder_regex(R"((\\)?%\{(!)?(\*)?((?:[A-Z]*[0-9]*_*)*)\}%)");
	auto				  words_begin	= std::sregex_iterator(format.begin(), format.end(), placeholder_regex);
	auto				  words_end		= std::sregex_iterator();
	std::set<std::string> specialTokenSet = { "MESSAGE", "LEVEL", "TIME" };

	// Uses regex to find all placeholders in the format string
	std::vector<std::string> staticTokens;
	std::vector<std::string> dynamicTokens;
	std::vector<std::string> specialTokens;
	for (std::sregex_iterator i = words_begin; i != words_end; ++i)
	{
		auto match = *i;

		if (match[1].matched)
			continue;	 // Skip escaped placeholders

		if (specialTokenSet.find(match[4].str()) != specialTokenSet.end())
		{
			if (match[3].matched)
			{
				if (match[2].matched)
				{
					if (std::find(staticTokens.begin(), staticTokens.end(), match[4].str()) == staticTokens.end() &&
						std::find(specialTokens.begin(), specialTokens.end(), match[4].str()) == specialTokens.end())
					{
						if (std::find(dynamicTokens.begin(), dynamicTokens.end(), match[4].str()) == dynamicTokens.end())
							dynamicTokens.emplace_back(match[4].str());	   // Add dynamic token
						else
							return false;	 // Duplicate token found
					}
					else
						return false;	 // Duplicate token found
					continue;			 // Special token is dynamic, skip adding to static tokens
				}
				else
					return false;	 // '*' not allowed for special tokens unless the override '!' is used
			}
			else
			{
				if (!match[2].matched)
				{
					if (std::find(specialTokens.begin(), specialTokens.end(), match[4].str()) == specialTokens.end())
						specialTokens.emplace_back(match[4].str());	   // Add dynamic token
					else
						return false;	 // Duplicate token found

					continue;	 // Special token not overriden, skip
				}
			}
		}
		else
		{
			if (match[2].matched)
				return false;	 // '!' not allowed for non special tokens
			if (match[3].matched)
			{

				if (std::find(staticTokens.begin(), staticTokens.end(), match[4].str()) == staticTokens.end())
				{
					if (std::find(dynamicTokens.begin(), dynamicTokens.end(), match[4].str()) == dynamicTokens.end())
						dynamicTokens.emplace_back(match[4].str());	   // Add dynamic token
					else
						return false;	 // Duplicate token found
				}
				else
					return false;	 // Duplicate token found

				continue;	 // Token is dynamic, skip adding to static tokens
			}
		}

		if (std::find(dynamicTokens.begin(), dynamicTokens.end(), match[4].str()) == dynamicTokens.end())
		{
			if (std::find(staticTokens.begin(), staticTokens.end(), match[4].str()) == staticTokens.end())
				staticTokens.emplace_back(match[4].str());
			else
				return false;	 // Duplicate token found
		}
		else
			return false;	 // Duplicate token found
	}

	bool isMessageInFormat = false;
	for (const auto& placeholder : staticTokens)
	{
		if (placeholder.size() == 0)
			return false;	 // Empty placeholder found

		if (placeholder == "MESSAGE")
			isMessageInFormat = true;	 // Message placeholder found in format string

		if (parameters.find(placeholder) == parameters.end())
		{
			return false;	 // Placeholder not found in parameters
		}
		else if (parameters.size() != 0)
		{
			if (placeholder == "MESSAGE")
				return false;	 // MESSAGE should not be in parameters
			else if (placeholder == "LEVEL")
				return false;	 // LEVEL should not be in parameters
			else if (placeholder == "TIME")
				return false;	 // TIME should not be in parameters
		}
	}
	if (isMessageInFormat)
		return true;	// All placeholders are valid and MESSAGE is present in format string
	else
		return false;	 // MESSAGE placeholder is mandatory
}

//! Set the log format and parameters.
/*!
	Sets the log message format and validates the provided parameters against the format string.
	The format string can contain several unique placeholders in the form of `%{TOKEN}%`,
	where 'TOKEN' is a string that contains only uppercase letters, numbers, and underscores.

	If the entire placeholder is preceded with a '\' (i.e. '\%{TOKEN}%') it will be treated as a literal string and not as a
   placeholder.

	Dynamic tokens are preceded with a '*' (i.e. '%{*TOKEN}%') to indicate that the parameter value will be provided at
	log time.

	The format string may contain special tokens:

	MESSAGE - This will be replaced with the actual log message. It is mandatory to include this tag in the format string.
	LEVEL - This will be replaced with the log level (INFO, DEBUG, WARN, ERROR, FATAL).
	TIME - This will be replaced with the current timestamp. In the future you may be able to specify the timestamp format.

	Special tokens (excluding MESSAGE) may be preceded with a '!' (i.e. '%{!TIME}%') to indicate that the special token
	should be treated as a standard token and its value provided in the parameters map.
	Additionally if the '!' is followed by a '*' (i.e. '%{!*TIME}%') it indicates that the token is dynamic, and the value will be
   provided at log time.

	The parameters map should contain key-value pairs where keys correspond to the tokens
	in the format string, and values are the strings that will replace the placeholder containing that token.
	Tokens in the parameters map should not include the surrounding '%{ }%', or any preceding '*' or '!' characters.
	Special tokens may only be in the parameter map if they are in this format: '%{!TOKEN}%'.
	The MESSAGE token should not be included in the parameters map as it is handled separately.
*/
/*!
	@param format - The log message format string.@param parameters -
	A map of parameter tokens to their corresponding values.
*/
void Logger::addFormat(const Format& format)
{
	validateFormatParameters(format.formatString, format.parameters);
	m_formats.push_back(format);
}

void Logger::setFormat(const size_t index, const Format& format)
{
	validateFormatParameters(format.formatString, format.parameters);

	m_formats[index] = format;
}

void Logger::updateFormatParameter(const size_t index, const std::string& token, const std::string& value)
{
	auto format = m_formats.at(index);
	if (format.parameters.find(token) != format.parameters.end())
	{
		format.parameters[token] = value;
	}
	else
	{
		// Handle error: token not found
	}
}

std::unique_ptr<SimpleLogger> SimpleLoggerFactory::create()
{
	auto logger = std::make_unique<SimpleLogger>();
	logger->addFormat(SimpleLogger::defaultFormat);
	logger->addDestination(Logger::LogDestination::CONSOLE);
	logger->setLogLevelFlags({ Logger::LogLevel::INFO,
							   Logger::LogLevel::DEBUG,
							   Logger::LogLevel::WARN,
							   Logger::LogLevel::ERROR,
							   Logger::LogLevel::FATAL });
	return logger;
}

}	 // namespace zaphod