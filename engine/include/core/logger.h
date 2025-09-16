#pragma once

#include <vector>
#include <string>

namespace zaphod::log
{
class Logger
{

  public:
	void log(const std::string& message) {};

  private:
	Logger()  = default;
	~Logger() = default;
	// Logger(const Logger&)			 = delete;
	// Logger& operator=(const Logger&) = delete;
	// Logger(Logger&&)				 = delete;
	// Logger& operator=(Logger&&)		 = delete;
	std::string				 m_format;
	std::vector<std::string> m_format_params;
};

class LoggerBuilder
{
  public:
	static void create() = 0;

  private:
	LoggerBuilder()	 = default;
	~LoggerBuilder() = default;

	void buildFormat(const std::string& format, std::vector<std::string>> format_params);
};

}	 // namespace zaphod::log