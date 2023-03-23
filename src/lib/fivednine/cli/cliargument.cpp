#include "cliargument.h"

using namespace fivednine;
using namespace fivednine::cli;

CommandLineArgument::CommandLineArgument(const std::string& argumentName, const std::string& argumentValue)
    : m_name(argumentName), m_value(argumentValue) {}

const std::string& CommandLineArgument::GetName() const
{
    return m_name;
}

std::string CommandLineArgument::AsString() const
{
    // ha ha !! you fool!! it is already a string !!!
    return m_value;
}