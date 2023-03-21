#include "appconfig.h"

#include <json/json.hpp>
#include <countdown/log/log.h>

#include <fstream>

using json = nlohmann::json;

using namespace countdown;

bool AppConfig::Parse(const std::string& configPath)
{
    // Parse configuration
    std::ifstream in(configPath);
    if (!in)
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Failed to load config file: %s", configPath.c_str());
        return false;
    }

    json configData = json::parse(in);
    m_shadersPath = configData["shaders_path"].get<std::string>();

    // const std::string TexturesPath = configData["textures_path"].get<std::string>();
    // const std::string GamesDbPath = configData["gamesdb_path"].get<std::string>();
    return true;
}

const std::string& AppConfig::GetShadersPath() const
{
    return m_shadersPath;
}