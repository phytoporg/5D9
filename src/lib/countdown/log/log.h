// log.h
//
// Logging functions and helper macros. Filtering has two dimensions: verbosity and
// zones. The former parameterizes the level of detail of logging which is enabled,
// the latter groups information emitted via logs into functional zones. Feel free
// to tack on additional zones as-needed.

#pragma once

#include <cstdint>

namespace countdown { namespace log {
    enum class LogVerbosity 
    {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Verbose,
        VeryVerbose,
        Max
    };
    static const LogVerbosity kDefaultLogVerbosity = LogVerbosity::Warning;

    enum class LogZone 
    {
        Default = 0,
        Render,
        Max
    };

    // Log configuration
    void SetLogVerbosity(LogVerbosity newLogVerbosity);

    // Default is stderr
    bool SetLogFile(const char* pLogFilePath);

    // All zones enabled by default
    void DisableZone(LogZone zone);
    void EnableZone(LogZone zone);
    bool GetIsZoneEnabled(LogZone zone);

    void LogLine(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...);
    void Log(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...);

    void LogAndFail(LogZone zone, const char* pFormatString, ...);
    void LogLineAndFail(LogZone zone, const char* pFormatString, ...);
}}

// Log macros

// Debug
#if defined(DEBUG)
    #define DEBUG_LOG_FATAL(Zone, String, ...) \
        countdown::log::LogAndFail(Zone, String, __VA_ARGS__);
    #define DEBUG_LOG_ERROR(Zone, String, ...) \
        countdown::log::Log(Zone, countdown::log::LogVerbosity::Error, String, __VA_ARGS__);
    #define DEBUG_LOG_WARNING(Zone, String, ...) \
        countdown::log::Log(Zone, countdown::log::LogVerbosity::Warning, String, __VA_ARGS__);
    #define DEBUG_LOG_INFO(Zone, String, ...) \
        countdown::log::Log(Zone, countdown::log::LogVerbosity::Info, String, __VA_ARGS__);
    #define DEBUG_LOG_VERBOSE(Zone, String, ...) \
        countdown::log::Log(Zone, countdown::log::LogVerbosity::Verbose, String, __VA_ARGS__);
    #define DEBUG_LOG_VERYVERBOSE(Zone, String, ...) \
        countdown::log::Log(Zone, countdown::log::LogVerbosity::VeryVerbose, String, __VA_ARGS__);

    #define DEBUG_LOGLINE_FATAL(Zone, String, ...) \
        countdown::log::LogLineAndFail(Zone, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_ERROR(Zone, String, ...) \
        countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Error, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_WARNING(Zone, String, ...) \
        countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Warning, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_INFO(Zone, String, ...) \
        countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Info, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_VERBOSE(Zone, String, ...) \
        countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Verbose, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_VERYVERBOSE(Zone, String, ...) \
        countdown::log::LogLine(Zone, countdown::log::LogVerbosity::VeryVerbose, String, __VA_ARGS__);
#else
    #define DEBUG_LOG_FATAL(Zone, String, ...) 
    #define DEBUG_LOG_ERROR(Zone, String, ...) 
    #define DEBUG_LOG_WARNING(Zone, String, ...) 
    #define DEBUG_LOG_INFO(Zone, String, ...) 
    #define DEBUG_LOG_VERBOSE(Zone, String, ...) 
    #define DEBUG_LOG_VERYVERBOSE(Zone, String, ...) 

    #define DEBUG_LOGLINE_FATAL(Zone, String, ...) 
    #define DEBUG_LOGLINE_ERROR(Zone, String, ...) 
    #define DEBUG_LOGLINE_WARNING(Zone, String, ...) 
    #define DEBUG_LOGLINE_INFO(Zone, String, ...) 
    #define DEBUG_LOGLINE_VERBOSE(Zone, String, ...)
    #define DEBUG_LOGLINE_VERYVERBOSE(Zone, String, ...)
#endif

// Release
#define RELEASE_LOG_FATAL(Zone, String, ...) \
    countdown::log::LogAndFail(Zone, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_ERROR(Zone, String, ...) \
    countdown::log::Log(Zone, countdown::log::LogVerbosity::Error, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_WARNING(Zone, String, ...) \
    countdown::log::Log(Zone, countdown::log::LogVerbosity::Warning, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_INFO(Zone, String, ...) \
    countdown::log::Log(Zone, countdown::log::LogVerbosity::Info, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_VERBOSE(Zone, String, ...) \
    countdown::log::Log(Zone, countdown::log::LogVerbosity::Verbose, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_VERYVERBOSE(Zone, String, ...) \
    countdown::log::Log(Zone, countdown::log::LogVerbosity::VeryVerbose, String __VA_OPT__(,) __VA_ARGS__)

#define RELEASE_LOGLINE_FATAL(Zone, String, ...) \
    countdown::log::LogLineAndFail(Zone, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_ERROR(Zone, String, ...) \
    countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Error, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_WARNING(Zone, String, ...) \
    countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Warning, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_INFO(Zone, String, ...) \
    countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Info, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_VERBOSE(Zone, String, ...) \
    countdown::log::LogLine(Zone, countdown::log::LogVerbosity::Verbose, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_VERYVERBOSE(Zone, String, ...) \
    countdown::log::LogLine(Zone, countdown::log::LogVerbosity::VeryVerbose, String __VA_OPT__(,) __VA_ARGS__)

#define LOG_DEFAULT countdown::log::LogZone::Default
#define LOG_RENDER countdown::log::LogZone::Render
