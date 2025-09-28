#pragma once

#include "util/flags.h"
#include "util/result.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace zaphod::logging
{

/**
 * @brief A base logger class providing core logger functionality.
 *
 * @details
 * The Logger class supports multiple log [formats](@ref Logger::Format),
 * [destinations](@ref Logger::LogDestination), and [levels](@ref Logger::LogLevel).
 * It allows for flexible configuration of log messages through format strings and parameters.
 * The logger can output log messages to various destinations such as console or file,
 * and supports different log levels to categorize the severity of log messages.
 * The class also provides methods to manage formats, destinations, and log levels.
 * Loggers are created using a factory pattern, with the `LoggerFactory` class
 *
 * @note The Logger class is a base class and must be extended for specific logging implementations.
 * @note The @ref log method must be implemented by derived classes.
 *
 * @see Logger::Format
 * @see Logger::LogDestination
 * @see Logger::LogLevel
 * @see Logger::LogLevelFlags
 * @see SimpleLogger
 *
 */
class Logger
{
  public:
	// struct Parameter
	//{
	//	std::string token;
	//	std::string value;
	//	bool		isDynamic = false;	  // If true, the value will be provided at log time
	// };

	/**
	 * @brief
	 * A log message format consisting of a format string and a set of parameters.
	 *
	 * @details
	 * The format string and parameters are validated upon construction and when updated.
	 *
	 */
	class Format
	{
	  public:
		/**
		 * @brief A map of string key-value pairs for storing parameters.
		 *
		 * This unordered_map associates tokens (as std::string) with their corresponding values (also as std::string).
		 * It is typically used for storing the values of static log tokens.
		 *
		 * @note The MESSAGE token should not be included in this map as it is handled separately.
		 *
		 * @note Special tokens (excluding MESSAGE) may only be included if they are overridden in the format
		 * string.
		 *
		 * @note The keys in this map should not include the surrounding '%{ }%', or any preceding '!' or '*' characters.
		 */
		typedef std::unordered_map<std::string, std::string> ParameterMap;

		/**
		 * @brief Construct a new Format object and validates it.
		 *
		 * @param formatString The log message format string.
		 * @param parameters The map of parameter tokens to their corresponding values.
		 */
		Format(const std::string& formatString, const ParameterMap& parameters) { validate(formatString, parameters); }

		/**
		 * @brief Get the current format string
		 *
		 * @return The current format string
		 */
		const std::string getFormatString() const { return m_formatString; }
		/**
		 * @brief Get current static tokens and their values
		 *
		 * @return The current map of static tokens and their values
		 */
		const ParameterMap& getStaticTokens() const { return m_staticTokens; }
		/**
		 * @brief Get the current dynamic tokens
		 *
		 * @return The list of dynamic tokens in the format string
		 */
		const std::vector<std::string> getDynamicTokens() const
		{
			return std::vector<std::string>(m_dynamicTokens.begin(), m_dynamicTokens.end());
		}
		/**
		 * @brief Get the current special tokens
		 *
		 * @return The list of special tokens in the format string
		 */
		const std::vector<std::string> getSpecialTokens() const
		{
			return std::vector<std::string>(m_specialTokens.begin(), m_specialTokens.end());
		}

		/**
		 * @brief Set the format string and validates it
		 *
		 * @param formatString The new format string to set
		 */
		void setFormatString(const std::string& formatString) { validateFormatString(formatString); }
		/**
		 * @brief Set the Parameters object
		 *
		 * @param parameters The new parameters map to set
		 */
		void setParameters(const ParameterMap& parameters) { validateStaticTokens(parameters); }
		/**
		 * @brief Set the Static Token object
		 *
		 * @param token The token to set
		 * @param value The value to set for the token
		 */
		void setStaticToken(const std::string& token, const std::string& value)
		{
			auto it = m_staticTokens.find(token);
			if (it != m_staticTokens.end())
				it->second = value;
		}
		/**
		 * @brief Check if the format is valid
		 *
		 * @return true if the format is valid, false otherwise
		 */
		bool isValid() const { return m_isValid; }

	  private:
		/**
		 * @brief The log message format string
		 *
		 * @details
		 * The format string can contain several unique placeholders in the form of `%{TOKEN}%`,
		 * where 'TOKEN' is a string that contains only uppercase letters, numbers, and underscores.
		 *
		 * If the entire placeholder is preceded with a '\' (i.e. '\\%{TOKEN}\%') it will be treated as
		 * a literal string and not as a placeholder.
		 *
		 * Dynamic tokens are preceded with a '*' (i.e. '%{*TOKEN}\%') to indicate that the parameter
		 * value will be provided at log time.
		 *
		 * The following special tokens are reserved unless overridden:
		 * - MESSAGE: The log message content.
		 * @note The MESSAGE token is mandatory and must be included in the format string.
		 * It also may not be overriden.
		 * @details
		 * - LEVEL: The log level of the log entry.
		 * - TIME: The timestamp of the log entry.
		 * .
		 *
		 * Special tokens can be overridden by prefixing them with '!' (i.e. '%{!LEVEL}\%').
		 * This indicates that the special token should be treated as a standard token and its
		 * value provided in the parameters map.
		 *
		 * Additionally, if the '!' is followed by a '*' (i.e. '%{!*TIME}\%') it indicates that the
		 * token is dynamic, and the value will be provided at log time.
		 *
		 * @note The format string is validated upon construction and when updated.
		 */
		std::string m_formatString;
		/**
		 * @brief The map of static tokens and their corresponding values
		 *
		 * @details
		 * This map stores the tokens that are not dynamic and their associated values.
		 * The keys in this map should not include the surrounding '%{ }\%', or any preceding
		 * '!' or '*' characters.
		 *
		 * It is automatically updated when the parameters are validated.
		 */
		ParameterMap m_staticTokens;
		/**
		 * @brief The set of dynamic tokens in the format string
		 *
		 * @details
		 * This set stores the tokens that are dynamic and will have their values provided at log time.
		 * The keys in this set should not include the surrounding '%{ }\%', or any preceding
		 * '!' or '*' characters.
		 *
		 * It is automatically updated when the format string is validated.
		 */
		std::set<std::string> m_dynamicTokens;
		/**
		 * @brief The set of special tokens in the format string
		 *
		 * @details
		 * This set stores the special tokens that are included in the format string.
		 * The keys in this set should not include the surrounding '%{ }\%', or any preceding
		 * '!' or '*' characters.
		 *
		 * It is automatically updated when the format string is validated.
		 */
		std::set<std::string> m_specialTokens;
		/**
		 * @brief Indicates whether the format is valid
		 *
		 * @details
		 * This boolean flag is set to true if the format string and parameters are valid,
		 * and false otherwise. It is updated during validation.
		 */
		bool m_isValid = false;

		/**
		 * @brief Validates the format string
		 *
		 * @details
		 * This method checks the format string for correctness.
		 * It ensures that all placeholders are well-formed, that the MESSAGE placeholder is present,
		 * and that special tokens are used correctly.
		 * It also checks for duplicate tokens and ensures that dynamic tokens are marked appropriately.
		 *
		 * @param formatString The format string to validate
		 * @return The Result of the validation\n
		 * If the format string is valid, returns Result::Code::SUCCESS\n
		 * If the format string is invalid, returns Result::Code::INVALID_ARGUMENT with an appropriate
		 * error message
		 */
		Result validateFormatString(const std::string& formatString);
		/**
		 * @brief Validates the static tokens
		 *
		 * @details
		 * This method checks the static tokens for correctness.
		 * It ensures that all tokens are well-formed, that there are no duplicate tokens,
		 * and that special tokens are not included unless overridden in the format string.
		 * It also checks that the MESSAGE token is not included in the static tokens.
		 *
		 * @param staticTokens The static tokens to validate
		 * @return The Result of the validation\n
		 * If the static tokens are valid, returns Result::Code::SUCCESS\n
		 * If the static tokens are invalid, returns Result::Code::INVALID_ARGUMENT with an appropriate
		 * error message
		 */
		Result validateStaticTokens(const ParameterMap& staticTokens);
		/**
		 * @brief Validates the format string and static tokens
		 *
		 * @details
		 * This method checks both the format string and static tokens for correctness.
		 * It combines the checks performed by validateFormatString and validateStaticTokens.
		 *
		 * @param formatString The format string to validate
		 * @param staticTokens The static tokens to validate
		 * @return The Result of the validation\n
		 * If both the format string and static tokens are valid, returns Result::Code::SUCCESS\n
		 * If either @ref validateFormatString or @ref validateStaticTokens fails,
		 * returns the corresponding Result from the failed validation
		 */
		Result validate(const std::string& formatString, const ParameterMap& staticTokens);
		/**
		 * @brief Validates @ref m_formatString and @ref m_staticTokens
		 *
		 * @details
		 * This method checks the @ref m_formatString and @ref m_staticTokens for correctness.
		 * It is run upon construction.
		 * It ensures that all static placeholders in the format string are accounted for in the parameters,
		 * and that the MESSAGE placeholder is present in the format string.
		 * It also checks for duplicate tokens and ensures that special tokens are used correctly.
		 *
		 * @return The Result of the validation
		 */
		Result validate() { return validate(m_formatString, m_staticTokens); }
	};

	/**
	 * @brief Log destination options
	 *
	 * @details
	 * - CONSOLE: Log messages will be output to the console (standard output).
	 * - FILE: Log messages will be written to a file.
	 */
	enum class LogDestination
	{
		CONSOLE,
		FILE
	};

	/**
	 * @brief Log level options
	 *
	 * @details
	 * - EMPTY: Required for use of Flags utility
	 * - INFO: Informational messages that highlight the progress of the application at coarse-grained level
	 * - DEBUG: Detailed information typically of interest only when diagnosing problems
	 * - WARN: Potentially harmful situations which still allow the application to continue running
	 * - ERROR: Error events that might still allow the application to continue running
	 * - FATAL: Very severe error events that will presumably lead the application to abort
	 *
	 */
	enum class LogLevel : uint32_t
	{
		EMPTY,
		INFO,
		DEBUG,
		WARN,
		ERROR,
		FATAL
	};

	/**
	 * @brief Construct a new Logger object
	 *
	 * Initializes the logger with no formats, no destinations, and no log levels enabled.
	 */
	Logger() = default;
	/**
	 * @brief Destroy the Logger object
	 *
	 */
	virtual ~Logger() = default;

	/**
	 * @brief Utilizes the Flags utility to manage enabled log levels.
	 */
	typedef Flags<LogLevel> LogLevelFlags;

	/**
	 * @brief Add a new log message format
	 *
	 * @param format The format to add
	 */
	void addFormat(const Format& format);
	/**
	 * @brief Remove a log message format at the specified index
	 *
	 * @param index The index of the format to remove
	 */
	void removeFormat(const size_t index);
	/**
	 * @brief Set a log message format at the specified index
	 *
	 * @param index The index of the format to set
	 * @param format The new format to set
	 */
	void setFormat(const size_t index, const Format& format);
	/**
	 * @brief Add a log destination
	 *
	 * @param destination The destination to add
	 */
	void addDestination(LogDestination destination) { m_destinations.emplace(destination); };
	/**
	 * @brief Remove a log destination
	 *
	 * @param destination The destination to remove
	 */
	void removeDestination(LogDestination destination) { m_destinations.erase(destination); };
	/**
	 * @brief Replace current log level flags with the provided flags
	 *
	 * @param flags The new log level flags to set
	 */
	void setLogLevelFlags(LogLevelFlags flags) { m_logLevelFlags = flags; };
	/**
	 * @brief Replace current log level flags with the provided levels
	 *
	 * @param levels The new log levels to set
	 */
	void setLogLevelFlags(std::vector<LogLevel> levels) { m_logLevelFlags = LogLevelFlags(levels); };
	/**
	 * @brief Enable or disable a specific log level
	 *
	 * @param level The log level to enable or disable
	 * @param enable True to enable the log level, false to disable (default: true)
	 */
	void setLogLevelFlag(LogLevel level, bool enable = true) { m_logLevelFlags.setFlag(level, enable); };

	/**
	 * @brief Update a specific format's parameter value at the specified index
	 *
	 * @param index The index of the format to update
	 * @param token The token of the parameter to update
	 * @param value The new value to set for the parameter
	 */
	void updateFormatParameter(const size_t index, const std::string& token, const std::string& value);
	/**
	 * @brief Update current log level flags by enabling the provided flags
	 *
	 * @param flags The log level flags to enable
	 */
	void updateLogLevelFlags(LogLevelFlags flags) { m_logLevelFlags.updateFlags(flags); };

  protected:
	/**
	 * @brief Log a message with the specified dynamic parameters, format index, and log level
	 *
	 * @note This method must be implemented by derived classes to handle the actual logging logic.
	 *
	 * @param message The log message content
	 * @param dynamicParameters The dynamic parameters to include in the log message
	 * @param formatIndex The index of the format to use for the log message (default: 0)
	 * @param level The log level of the message (default: INFO)
	 */
	virtual void log(const std::string&				 message,
					 const std::vector<std::string>& dynamicParameters,
					 size_t							 formatIndex,
					 LogLevel						 level) = 0;

	/**
	 * @brief The set of log destinations currently enabled
	 */
	std::set<LogDestination> m_destinations;
	/**
	 * @brief The list of log message formats currently defined
	 */
	std::vector<Format> m_formats;
	/**
	 * @brief The flags representing the currently enabled log levels
	 */
	LogLevelFlags m_logLevelFlags;
};

/**
 * @brief A simple logger implementation of the Logger base class.
 *
 * @details
 * The SimpleLogger class provides basic logging functionality for INFO and WARN log levels.
 * It uses a default log message format that includes the log level, timestamp, and message content.
 * The logger outputs log messages to the console by default, but can be configured
 * to log to other destinations as needed.
 */
class SimpleLogger: public Logger
{
  public:
	SimpleLogger()			 = default;
	~SimpleLogger() override = default;

	/**
	 * @brief Log an informational message.
	 *
	 * @details
	 * This method logs a message with the INFO log level.
	 * It uses the default log format and outputs to the configured destinations.
	 *
	 * @param message The informational message to log.
	 */
	void info(std::string message);
	/**
	 * @brief Log a debug message.
	 *
	 * @details
	 * This method logs a message with the DEBUG log level.
	 * It uses the default log format and outputs to the configured destinations.
	 *
	 * @param message
	 */
	void debug(std::string message);

	/**
	 * @brief The default log message format string for SimpleLogger.
	 *
	 * @details
	 * The default format string includes the source of the log message, log level,
	 * timestamp, and message content.
	 *
	 * The format string is: "[%{SOURCE}%][%{LEVEL}%][%{TIME}%]: %{MESSAGE}%"
	 *
	 * This format string produces log messages in the following format:
	 * [SOURCE][LEVEL][TIME]: MESSAGE
	 *
	 * For example:\n
	 * [MyApp][INFO][2024-10-05 14:23:45]: Application started successfully.\n
	 * [MyApp][DEBUG][2024-10-05 14:23:45]: Debugging application flow.
	 *
	 */
	inline const static std::string defaultFormatString = "[%{SOURCE}%][%{LEVEL}%][%{TIME}%]: %{MESSAGE}%";

  protected:
  /**
   * @brief Log a message with the specified dynamic parameters, format index, and log level
   * 
   * @details
   * This method overrides the base Logger's @ref log method to handle logging for
   * INFO and DEBUG log levels.
   * It uses the default log format and outputs to the configured destinations.
   * 
   * @param message The log message content
   * @param dynamicParameters The dynamic parameters to include in the log message
   * @param formatIndex The index of the format to use for the log message
   * @param level The log level of the message
   */
	void log(const std::string&				 message,
			 const std::vector<std::string>& dynamicParameters,
			 size_t							 formatIndex,
			 LogLevel						 level) override;
};

/**
 * @brief A logger specialized for logging errors, warnings, and fatal messages.
 *
 * @details
 * The ErrorLogger class extends the base Logger class to provide
 * specialized logging methods for warnings, errors, and fatal messages.
 * It uses a predefined log message format that includes the source of the error,
 * error level, timestamp, function name, and message content.
 * The logger outputs log messages to the console by default, but can be configured
 */
class ErrorLogger: public Logger
{
  public:
	ErrorLogger()  = default;
	~ErrorLogger() = default;

	/**
	 * @brief Log a warning message.
	 *
	 * @details
	 * This method logs a message with the WARN log level.
	 * It uses the default log format and outputs to the configured destinations.
	 *
	 * Additionally, the warning message will be colored yellow in console output.
	 *
	 * @param message The warning message to log.
	 */
	void warn(std::string message);
	/**
	 * @brief Log an error message.
	 *
	 * @details
	 * This method logs a message with the ERROR log level.
	 * It uses the default log format and outputs to the configured destinations.
	 *
	 * Additionally, the error message will be colored orange in console output.
	 *
	 * @param message The error message to log.
	 */
	void error(std::string message);
	/**
	 * @brief Log a fatal error message.
	 *
	 * @details
	 * This method logs a message with the FATAL log level.
	 * It uses the default log format and outputs to the configured destinations.
	 *
	 * Additionally, the fatal error message will be colored red in console output.
	 *
	 * @param message The fatal error message to log.
	 */
	void fatal(std::string message);

	/**
	 * @brief The default log message format string for ErrorLogger.
	 *
	 * @details
	 * The default format string includes the error level, timestamp,
	 * source of the error, function name, and message content.
	 *
	 * The format string is: "[%{LEVEL}%][%{TIME}%]{%{SOURCE}%->%{FUNCTION}%}: %{MESSAGE}%"
	 *
	 * This format string produces log messages in the following format:
	 * [LEVEL][TIME]{SOURCE->FUNCTION}: MESSAGE
	 * * For example:\n
	 * [WARN][2024-10-05 14:23:45]{MyApp->initialize}: Potential issue detected.\n
	 * [ERROR][2024-10-05 14:23:45]{MyApp->processData}: An error occurred while processing data.\n
	 * [FATAL][2024-10-05 14:23:45]{MyApp->connectToDatabase}: Fatal error - unable to connect to database.
	 *
	 */
	inline const static std::string defaultFormatString = "[%{LEVEL}%][%{TIME}%]{%{SOURCE}%->%{FUNCTION}%}: %{MESSAGE}%";

	/**
	 * @brief Log a message with the specified dynamic parameters, format index, and log level
	 * 
	 * @details
	 * This method overrides the base Logger's @ref log method to handle logging for
	 * WARN, ERROR, and FATAL log levels.
	 * It uses the default log format and outputs to the configured destinations.
	 * Additionally, it applies color coding to the log messages based on the log level:
	 * - WARN messages are colored yellow.
	 * - ERROR messages are colored orange.
	 * - FATAL messages are colored red.
	 * 
	 * @param message The log message content
	 * @param dynamicParameters The dynamic parameters to include in the log message
	 * @param formatIndex The index of the format to use for the log message
	 * @param level The log level of the message
	 * 
	 */
  protected:
	void log(const std::string&				 message,
			 const std::vector<std::string>& dynamicParameters,
			 size_t							 formatIndex,
			 LogLevel						 level) override;
};

/**
 * @brief A factory interface for creating logger instances.
 *
 * @tparam T The type of logger to create (must be derived from Logger).
 *
 * @details
 * The LoggerFactory class defines a common interface for creating logger instances.
 * It uses the factory design pattern to allow for flexible creation of different logger types.
 * Concrete implementations of this interface must provide the create() method to instantiate
 * and return a unique pointer to the specified logger type.
 * This design allows for easy extension and customization of logger creation logic.
 * For example, different factories can be implemented to create loggers with specific
 * configurations or behaviors.
 * @see SimpleLoggerFactory
 */
template<typename T>
class LoggerFactory
{
  public:
	virtual ~LoggerFactory() = default;
	/**
	 * @brief Create a new logger instance.
	 *
	 * @return A unique pointer to the created logger instance.
	 */
	virtual std::unique_ptr<T> create() = 0;
};

/**
 * @brief A factory for creating SimpleLogger instances.
 *
 * @details
 * The SimpleLoggerFactory class implements the LoggerFactory interface to create
 * instances of SimpleLogger. It provides a concrete implementation of the create()
 * method, which initializes a SimpleLogger with default settings, including a
 * predefined log format and enabled log levels.
 * This factory can be used to easily obtain configured SimpleLogger instances
 * for use in applications.
 * @see LoggerFactory
 * @see SimpleLogger
 */
class SimpleLoggerFactory: public LoggerFactory<SimpleLogger>
{
  public:
	/**
	 * @brief Create a new SimpleLogger instance with default settings.
	 *
	 * @details
	 * This method initializes a SimpleLogger with a default log format that includes
	 * the source, log level, timestamp, and message content. It also enables INFO and DEBUG
	 * log levels and adds the console as a log destination by default.
	 *
	 * @return A unique pointer to the created SimpleLogger instance.
	 */
	std::unique_ptr<SimpleLogger> create() override;
};
}	 // namespace zaphod::logging