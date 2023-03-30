#include "daemonconfig.h"
#include "daemonrunner.h"
#include <common/cli/cliargumentparser.h>
#include <common/log/log.h>

#include <string>

#include <unistd.h>

using namespace common;
using namespace common::cli;

int main(int argc, char** argv)
{
    // TODO: Do daemon things like chdir("/") and umask(0) and fork()

    // Initialize logging
    log::SetLogVerbosity(log::LogVerbosity::Warning);
    log::RegisterLogZone(LOG_DEFAULT, true, "Default");

    CommandLineArgumentParser argumentParser(argc, argv);
    const std::string& ConfigPath = argumentParser.GetArgument("config");
    if (ConfigPath.empty())
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Config path is empty");
        return -1;
    }

    DaemonConfig config;
    if (!config.Parse(ConfigPath))
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to parse config");
        return -1;
    }

    // Start the launcher
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Cannot start launcher, fork failed");
        return -1;
    }
    else if (pid > 0)
    {
        // Parent process
        DaemonRunner runner(config);
        runner.Run();
    }
    else
    {
        // Child process
        const std::string& LauncherPath = config.GetLauncherPath();
        execl(LauncherPath.c_str(), LauncherPath.c_str(), "--config", config.GetLauncherConfig().c_str(), 0);
    }

    return 0;
}
