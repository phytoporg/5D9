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
    if (!configData.contains("textures_path"))
    {
        RELEASE_LOG_ERROR(
            LOG_DEFAULT,
            "Configuration file %s is missing required key: 'textures_path'",
            m_texturesPath);
            return false;
    }
    m_texturesPath = configData["textures_path"].get<std::string>();

    if (!configData.contains("shaders_path"))
    {
        RELEASE_LOG_ERROR(
            LOG_DEFAULT,
            "Configuration file %s is missing required key: 'shaders_path'",
            m_shadersPath);
            return false;
    }
    m_shadersPath = configData["shaders_path"].get<std::string>();

    // TODO:
    // const std::string GamesDbPath = configData["gamesdb_path"].get<std::string>();

    m_parsed = true;
    return true;
}

bool AppConfig::GetIsParsed() const
{
    return m_parsed;
}

const std::string& AppConfig::GetShadersPath() const
{
    return m_shadersPath;
}

const std::string& AppConfig::GetTexturesPath() const
{
    return m_texturesPath;
}