#include "core/logger.h"

#include <regex>
#include <set>

namespace zaphod::logging
{
Result Logger::Format::validateFormatString(const std::string& formatString)
{
	return Result(Result::Code::SUCCESS);
}

Result Logger::Format::validateStaticTokens(const Logger::Format::ParameterMap& staticTokens)
{
	return Result(Result::Code::SUCCESS);
}

Result Logger::Format::validate(const std::string& formatString, const Logger::Format::ParameterMap& staticTokens)
{
	Result formatResult		 = validateFormatString(formatString);
	Result staticTokenResult = validateStaticTokens(staticTokens);

	if (!formatResult.isSuccess())
		return formatResult;
	if (!staticTokenResult.isSuccess())
		return staticTokenResult;
	return Result(Result::Code::SUCCESS);
}

bool validateFormatParameters(const std::string& format, const std::unordered_map<std::string, std::string>& parameters)
{
	std::regex			  placeholder_regex(R"((\\)?%\{(!)?(\*)?((?:[A-Z]*[0-9]*_*)*)\}%)");
	auto				  words_begin	  = std::sregex_iterator(format.begin(), format.end(), placeholder_regex);
	auto				  words_end		  = std::sregex_iterator();
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

void Logger::addFormat(const Format& format)
{
	m_formats.push_back(format);
}

void Logger::setFormat(const size_t index, const Format& format)
{
	m_formats[index] = format;
}

void Logger::updateFormatParameter(const size_t index, const std::string& token, const std::string& value)
{
	auto format = m_formats.at(index);
	format.setStaticToken(token, value);
}

std::unique_ptr<SimpleLogger> SimpleLoggerFactory::create()
{
	auto logger = std::make_unique<SimpleLogger>();

	Logger::Format defaultFormat(std::string(SimpleLogger::defaultFormatString), Logger::Format::ParameterMap {});
	logger->addFormat(defaultFormat);
	logger->addDestination(Logger::LogDestination::CONSOLE);
	logger->setLogLevelFlags({ Logger::LogLevel::INFO, Logger::LogLevel::DEBUG });
	return std::move(logger);	
}

}	 // namespace zaphod::logging