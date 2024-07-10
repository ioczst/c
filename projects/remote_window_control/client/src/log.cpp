#pragma once
#include <log.h>
#include <fmt/core.h>

extern std::string NotificationMessage;

void PrintLog(std::string message)
{
    NotificationMessage = fmt::format("{0}\n{1}", std::string(NotificationMessage), message);
}