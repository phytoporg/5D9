#pragma once

#include <string>

class AppConfig
{
    public:
        bool Parse(const std::string& configPath);

        bool GetIsParsed() const;
        const std::string& GetShadersPath() const;
        const std::string& GetTexturesPath() const;
        const std::string& GetGamesDbPath() const;

    private:
        bool m_parsed = false;
        std::string m_shadersPath;
        std::string m_texturesPath;
        std::string m_gamesDbPath;
};