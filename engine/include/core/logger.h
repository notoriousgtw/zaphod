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

	class Format
	{
	  public:
		typedef std::unordered_map<std::string, std::string> ParameterMap;

		Format(std::string& formatString, ParameterMap& parameters) { validate(formatString, parameters); }

		std::string					   getFormatString() const { return m_formatString; }
		const ParameterMap&			   getStaticTokens() const { return const m_staticTokens; }
		const std::vector<std::string> getDynamicTokens() const { return m_dynamicTokens; }
		const std::vector<std::string> getSpecialTokens() const { return m_dynamicTokens; }

		void setFormatString(const std::string& formatString) { validate(formatString); }
		void setParameters(const ParameterMap& parameters) { validate(parameters); }
		void setStaticToken(const std::string& token, const std::string& value)
		{
			auto it = m_staticTokens.find(token);
			if (it != m_staticTokens.end())
				it->second = value;
		}

		bool isValid() const { return m_isValid; }

	  private:
		std::string			  m_formatString;
		ParameterMap		  m_staticTokens;
		std::set<std::string> m_dynamicTokens;
		std::set<std::string> m_specialTokens;
		bool				  m_isValid = false;

		void validate();
		void validate(std::string formatString);
		void validate(ParameterMap parameters);
		void validate(std::string formatString, ParameterMap parameters);
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
	void setLogLevelFlags(LogLevelFlags flags) { m_logLevelFlags = flags; };
	void setLogLevelFlags(std::vector<LogLevel> levels) { m_logLevelFlags = LogLevelFlags(levels); };
	void setLogLevelFlag(LogLevel level, bool enable = true) { m_logLevelFlags.setFlag(level, enable); };

	void updateFormatParameter(const size_t index, const std::string& token, const std::string& value);
	void updateLogLevelFlags(LogLevelFlags flags) { m_logLevelFlags.updateFlags(flags); };

  protected:
	void log(const std::string& message, const std::vector<std::string>& dynamicParameters, size_t formatIndex, LogLevel level);

	std::set<LogDestination> m_destinations;
	std::vector<Format>		 m_formats;
	LogLevelFlags			 m_logLevelFlags;
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

class ErrorLogger: public Logger
{
  public:
	ErrorLogger() = default;
	~ErrorLogger() = default;

	void					   info(std::string message);
	void					   debug(std::string message);
	void					   warn(std::string message);
	void					   error(std::string message);
	void					   fatal(std::string message);
	inline const static std::string defaultFormatString = "[ErrorHandler][%{LEVEL}%][%{TIME}%]:{%{FUNCTION}%}->%{MESSAGE}%";
}

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