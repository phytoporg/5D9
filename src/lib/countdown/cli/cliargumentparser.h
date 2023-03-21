#pragma once

#include <vector>

#include "cliargument.h"

namespace countdown { namespace cli {
    class CommandLineArgumentParser
    {
    public:
        CommandLineArgumentParser(int argc, char** argv);

        const CommandLineArgument* FindArgument(const std::string& argumentName) const;

    private:
        std::vector<CommandLineArgument> m_parsedArguments;
    };
}}