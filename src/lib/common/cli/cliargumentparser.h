#pragma once

#include <vector>

#include "cliargument.h"

namespace common { namespace cli {
    class CommandLineArgumentParser
    {
    public:
        CommandLineArgumentParser(int argc, char** argv);

        std::string GetArgument(const std::string& argumentName, const std::string& defaultValue = "") const;

        const CommandLineArgument* FindArgument(const std::string& argumentName) const;

    private:
        std::vector<CommandLineArgument> m_parsedArguments;
    };
}}