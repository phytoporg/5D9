#pragma once

#include <string>

class AppConfig
{
    public:
        bool Parse(const std::string& configPath);

        bool GetIsParsed() const;
        const std::string& GetShadersPath() const;

    private:
        bool m_parsed = false;
        std::string m_shadersPath;
};