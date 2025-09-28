#pragma once

#include "core/logger.h"

#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace zaphod::events
{
class EventBase
{
  public:
	virtual ~EventBase()					= default;
	virtual std::type_index getType() const = 0;

	template<typename T>
	bool isType() const
	{
		return getType() == typeid(T);
	}

	template<typename T>
	const T& as() const
	{
		if (!isType<T>())
			throw std::bad_cast();
		return static_cast<const T&>(*this);
	}
};

class LogEvent: public EventBase
{
  public:
	using Logger = zaphod::logging::Logger;
	LogEvent(const std::string&				 message,
			 const std::vector<std::string>& dynamicParameters,
			 Logger::LogLevel				 level,
			 size_t							 formatIndex = 0):
		m_message(message), m_dynamicParameters(dynamicParameters), m_formatIndex(formatIndex), m_level(level)
	{
	}

	std::type_index getType() const override { return typeid(LogEvent); }

	const std::string&				getMessage() const { return m_message; }
	const std::vector<std::string>& getDynamicParameters() const { return m_dynamicParameters; }
	size_t							getFormatIndex() const { return m_formatIndex; }
	Logger::LogLevel				getLogLevel() const { return m_level; }

  private:
	std::string				 m_message;
	std::vector<std::string> m_dynamicParameters;
	size_t					 m_formatIndex;
	Logger::LogLevel		 m_level;
};
}	 // namespace zaphod::events