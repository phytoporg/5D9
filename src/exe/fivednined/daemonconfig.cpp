#include "daemonconfig.h"
#include <json/json.hpp>
#include <fstream>

#include <common/log/log.h>

using json = nlohmann::json;

bool DaemonConfig::Parse(const std::string& configPath)
{
    std::ifstream configIn(configPath);
    if (!configIn.good())
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Could not read config from file path %s", configPath.c_str());
        return false;
    }

    json configData = json::parse(configIn);
    if (!configData.contains("launcher_path"))
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Config missing required key: 'launcher_path'");
        return false;
    }
    m_launcherPath = configData["launcher_path"].get<std::string>();

    if (!configData.contains("launcher_config"))
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Config missing required key: 'launcher_config'");
        return false;
    }
    m_launcherConfig = configData["launcher_config"].get<std::string>();

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
const std::string& DaemonConfig::GetLauncherConfig() const
{
    return m_launcherConfig;
}
