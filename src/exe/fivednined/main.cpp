#include "daemonconfig.h"
#include <common/cli/cliargumentparser.h>

#include <string>

using namespace common;
using namespace common::cli;

int main(int argc, char** argv)
{
    // TODO: Do daemon things like chdir("/") and umask(0) and fork()

    CommandLineArgumentParser argumentParser(argc, argv);

    // TODO: Figure out where this belongs on an "prod" install
    const std::string& ConfigPath = argumentParser.GetArgument("config");
    if (ConfigPath.empty())
    {
        // TODO: Log a failure
        return -1;
    }

    return -1;
}
