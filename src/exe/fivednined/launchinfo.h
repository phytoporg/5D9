#pragma once

#include <string>

struct LaunchInfo
{
    // Human-readable process identifier
    std::string Name;

    // Shell-executable command for launching a process
    std::string Command;
};