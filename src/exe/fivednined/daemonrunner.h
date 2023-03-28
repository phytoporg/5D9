#pragma once

class DaemonConfig;

class DaemonRunner
{
public:
    DaemonRunner(const DaemonConfig& config);
    void Run();

private:
    const DaemonConfig& m_config;
};