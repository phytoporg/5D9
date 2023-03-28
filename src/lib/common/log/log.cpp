#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <stdexcept>

#include <vector>
#include <string>

using namespace common;
using namespace common::log;

static LogVerbosity s_CurrentVerbosity = kDefaultLogVerbosity;
static std::vector<bool> ZoneMask;
static std::vector<std::string> ZoneIndexToString;
static FILE* s_pFile = stderr;

namespace 
{
    const char* ZoneToString(LogZone zone)
    {
        // TODO: Not thread safe
        const auto Index = static_cast<uint8_t>(zone);
        if (Index >= ZoneMask.size())
        {
            return nullptr;
        }

        return ZoneIndexToString[Index].c_str();
    }
}

void common::log::SetLogVerbosity(LogVerbosity newLogVerbosity)
{
    s_CurrentVerbosity = newLogVerbosity;
}

bool common::log::SetLogFile(const char* pLogFilePath)
{
    if (s_pFile != stderr && s_pFile != stdout)
    {
        fclose(s_pFile);
        s_pFile = nullptr;
    }

    FILE* pFile = fopen(pLogFilePath, "w");
    if (!pFile) { return false; }

    s_pFile = pFile;
    return true;
}

bool common::log::RegisterLogZone(LogZone zoneIndex, bool enableZone, const char* pZoneName)
{
    if (zoneIndex >= ZoneMask.size())
    {
        ZoneMask.resize(zoneIndex + 1);
        ZoneIndexToString.resize(zoneIndex + 1);
    }
    else if(!ZoneIndexToString[zoneIndex].empty())
    {
        // Don't redefine an existing zone
        return false;
    }

    ZoneMask[zoneIndex] = enableZone;
    ZoneIndexToString[zoneIndex] = pZoneName;

    return true;
}

void common::log::DisableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = false;
}

void common::log::EnableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = true;
}

bool common::log::GetIsZoneEnabled(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    return ZoneMask[Index];
}

void common::log::LogLine(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
{
    if (verbosity > s_CurrentVerbosity || !GetIsZoneEnabled(zone))
    {
        return;
    }

    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    perror(formatted);
    va_end(args);

    fprintf(s_pFile, "[%s] %s\n", ZoneToString(zone), formatted);
}

void common::log::Log(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
{
    if (verbosity > s_CurrentVerbosity || !GetIsZoneEnabled(zone))
    {
        return;
    }

    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    perror(formatted);
    va_end(args);

    fprintf(s_pFile, "[%s] %s", ZoneToString(zone), formatted);
}

void common::log::LogAndFail(LogZone zone, const char* pFormatString, ...)
{
    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    perror(formatted);
    va_end(args);

    fprintf(s_pFile, "[%s] %s", ZoneToString(zone), formatted);

    throw std::exception();
}

void common::log::LogLineAndFail(LogZone zone, const char* pFormatString, ...)
{
    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    perror(formatted);
    va_end(args);

    fprintf(s_pFile, "[%s] %s\n", ZoneToString(zone), formatted);

    throw std::exception();
}
