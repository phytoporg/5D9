#include "daemonconfig.h"
#include <json/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool DaemonConfig::Parse(const std::string& configPath)
{
    std::ifstream configIn(configPath);
    if (!configIn.good())
    {
        // TODO: Log an error
        return false;
    }

    json configData = json::parse(configIn);
    if (!configData.contains("launcher_path"))
    {
        // TODO: Log an error
        return false;
    }

    m_launcherPath = configData["launcher_path"].get<std::string>();
    m_parsed = true;
    return true;
}

bool DaemonConfig::GetIsParsed() const
{
    return m_parsed;
}

const std::string& DaemonConfig::GetLauncherPath() const
{
    return m_launcherPath;
}