#pragma once

#include <string>

namespace common { namespace cli {
    class CommandLineArgument
    {
    public:
        // For flag arguments
        explicit CommandLineArgument(const std::string& argumentName);

        // For arguments with values
        CommandLineArgument(const std::string& argumentName, const std::string& argumentValue);

        const std::string& GetName() const;

        std::string AsString() const;

    private:
        const std::string m_name;
        const std::string m_value;
        bool m_isFlag = false;
    };
}}