#include "LogManager.h"
#include "spdlog/sinks/stdout_color_sinks.h"

LogManager::LogManager()
{
	m_Logger = spdlog::stdout_color_mt("console");
	m_Logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l:%s%$] %v");
}

LogManager& LogManager::Get()
{
	static LogManager manager;
	return manager;
}

std::shared_ptr<spdlog::logger> LogManager::GetLogger()
{
	return m_Logger;
}
