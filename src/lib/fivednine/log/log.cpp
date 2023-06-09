#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <stdexcept>

using namespace fivednine;
using namespace fivednine::log;

static LogVerbosity s_CurrentVerbosity = kDefaultLogVerbosity;
static bool ZoneMask[static_cast<uint8_t>(LogZone::Max)] = {};
static FILE* s_pFile = stderr;

namespace 
{
    const char* ZoneToString(LogZone zone)
    {
        // TODO: Not thread safe
        const uint8_t Index = static_cast<uint8_t>(zone);
        static const char* LUT[] = { 
            "Default",
            "Render",
            "API",
        };

        static_assert(sizeof(LUT) / sizeof(LUT[0]) == static_cast<int>(LogZone::Max), "Log zone LUT size does not match enum");

        return LUT[Index];
    }
}

void fivednine::log::SetLogVerbosity(LogVerbosity newLogVerbosity)
{
    s_CurrentVerbosity = newLogVerbosity;
}

bool fivednine::log::SetLogFile(const char* pLogFilePath)
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

void fivednine::log::DisableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = false;
}

void fivednine::log::EnableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = true;
}

bool fivednine::log::GetIsZoneEnabled(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    return ZoneMask[Index];
}

void fivednine::log::LogLine(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
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

void fivednine::log::Log(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
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

void fivednine::log::LogAndFail(LogZone zone, const char* pFormatString, ...)
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

void fivednine::log::LogLineAndFail(LogZone zone, const char* pFormatString, ...)
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
