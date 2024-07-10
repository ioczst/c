#pragma once
#include <common.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <fmt/core.h>
#include <fmt/color.h>

const std::string CurrentDatetime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

std::vector<std::string> SplitString(std::string str, char separator)
{
    std::vector<std::string> stringVector;

    std::istringstream iss(str.c_str());
    std::string stringItem;
    while (std::getline(iss, stringItem, separator))
    {
        stringVector.push_back(stringItem);
    }
    return stringVector;
}

void PrintLog(std::string msg)
{
    std::string date = fmt::format(fg(fmt::rgb(255, 255, 0)), CurrentDatetime());
    fmt::println("{0} {1}", date, msg);
}