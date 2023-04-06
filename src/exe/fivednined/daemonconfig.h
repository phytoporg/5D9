#pragma once

#include <string>

class DaemonConfig
{
    public:
        bool Parse(const std::string& configPath);

        bool GetIsParsed() const;
        const std::string& GetLauncherPath() const;
        const std::string& GetLauncherConfig() const;

    private:
        bool m_parsed = false;
        std::string m_launcherPath;
        std::string m_launcherConfig;
};