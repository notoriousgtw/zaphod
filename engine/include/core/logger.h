#pragma once

#include "util/flags.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace zaphod::log
{

class Logger
{
  public:
	enum class LogDestination
	{
		CONSOLE,
		FILE
	};

	enum class LogLevel : uint32_t
	{
		EMPTY,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	typedef Flags<LogLevel> LogLevelFlags;

	void setFormat(const std::string& format, const std::unordered_map<std::string, std::string> parameters);
	void addDestination(LogDestination destination) { m_destinations.emplace(destination); };
	void removeDestination(LogDestination destination) { m_destinations.erase(destination); };
	void setLogLevelFlags(LogLevelFlags flags) { m_log_level_flags = flags; };
	void setLogLevelFlag(LogLevel level, bool enable = true) { m_log_level_flags.setFlag(level, enable); };

	void updateFormatParameter(const std::string& token, const std::string& value);
	void updateLogLevelFlags(LogLevelFlags flags) { m_log_level_flags.updateFlags(flags); };

  private:
	Logger() = default;
	//~Logger() = default;
	// Logger(const Logger&)			 = delete;
	// Logger& operator=(const Logger&) = delete;
	// Logger(Logger&&)				 = delete;
	// Logger& operator=(Logger&&)		 = delete;

	void log(const std::vector<std::string>& parameters, LogLevel level);

	std::string									 m_format;
	std::unordered_map<std::string, std::string> m_parameters;
	std::set<LogDestination>					 m_destinations;
	LogLevelFlags								 m_log_level_flags;
};

class LoggerBuilder
{
  public:
	virtual void   buildFormat()	   = 0;
	virtual void   buildDestinations() = 0;
	virtual void   buildLogLevel()	   = 0;
	virtual Logger construct()		   = 0;
};

class LoggerDirector
{
  public:
	Logger createLogger(LoggerBuilder& builder)
	{
		builder.buildFormat();
		builder.buildDestinations();
		builder.buildLogLevel();
		return builder.construct();
	}
};

class StandardLoggerBuilder: public LoggerBuilder
{
  public:
	StandardLoggerBuilder(std::string app_name);
	void   buildFormat() override;
	void   buildDestinations() override;
	void   buildLogLevel() override;
	Logger construct() override;

  private:
	Logger m_logger;
	std::string m_format = "[%{APP_NAME}%]-{%{TAG}%} %{MESSAGE}%";
};

class ErrorLoggerBuilder: public LoggerBuilder
{
  public:
	void   buildFormat() override;
	void   buildDestinations() override;
	void   buildLogLevel() override;
	Logger construct() override;

  private:
	Logger m_logger;
};
}	 // namespace zaphod::log