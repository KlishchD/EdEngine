#pragma once
#include "spdlog/spdlog.h"

class LogManager
{
public:
    static LogManager& Get();
    std::shared_ptr<spdlog::logger> GetLogger();
private:
    std::shared_ptr<spdlog::logger> m_Logger;

    LogManager();
};
