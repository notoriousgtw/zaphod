#pragma once

#include "util/flags.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace zaphod
{

class Logger
{
  public:
	// struct Parameter
	//{
	//	std::string token;
	//	std::string value;
	//	bool		isDynamic = false;	  // If true, the value will be provided at log time
	// };

	struct Format
	{
		std::string									 formatString;
		std::unordered_map<std::string, std::string> parameters;
	};

	enum class LogDestination
	{
		CONSOLE,
		FILE
	};

	enum class LogLevel : uint32_t
	{
		EMPTY,
		INFO,
		DEBUG,
		WARN,
		ERROR,
		FATAL
	};
	Logger()		  = default;
	virtual ~Logger() = default;

	typedef Flags<LogLevel> LogLevelFlags;

	void addFormat(const Format& format);
	void removeFormat(const size_t index);
	void setFormat(const size_t index, const Format& format);
	void addDestination(LogDestination destination) { m_destinations.emplace(destination); };
	void removeDestination(LogDestination destination) { m_destinations.erase(destination); };
	void setLogLevelFlags(LogLevelFlags flags) { m_log_level_flags = flags; };
	void setLogLevelFlags(std::vector<LogLevel> levels) { m_log_level_flags = LogLevelFlags(levels); };
	void setLogLevelFlag(LogLevel level, bool enable = true) { m_log_level_flags.setFlag(level, enable); };

	void updateFormatParameter(const size_t index, const std::string& token, const std::string& value);
	void updateLogLevelFlags(LogLevelFlags flags) { m_log_level_flags.updateFlags(flags); };

  protected:
	void log(const std::string& message, const std::vector<std::string>& parameters, size_t format_index, LogLevel level);

	std::set<LogDestination> m_destinations;
	std::vector<Format>		 m_formats;
	LogLevelFlags			 m_log_level_flags;
};

class SimpleLogger: public Logger
{
  public:
	SimpleLogger()			 = default;
	~SimpleLogger() override = default;

	void					   info(std::string message);
	void					   debug(std::string message);
	void					   warn(std::string message);
	void					   error(std::string message);
	void					   fatal(std::string message);
	inline const static Format defaultFormat = { "%{TIME}% [\\%{LEVEL}%]: %{MES_SAGE}% %{*P1}% %{!LEVEL}%",
												 { { "MES_SAGE", "A" }, { "LEVEL", "IN" } } };

  private:
};

template<typename T>
class LoggerFactory
{
  public:
	virtual ~LoggerFactory()			= default;
	virtual std::unique_ptr<T> create() = 0;
};

class SimpleLoggerFactory: public LoggerFactory<SimpleLogger>
{
  public:
	std::unique_ptr<SimpleLogger> create() override;
};
}	 // namespace zaphod