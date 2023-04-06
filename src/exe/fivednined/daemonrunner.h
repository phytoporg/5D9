#pragma once

#include "launchinfo.h"
#include "protocol/message.h"
#include <vector>

class DaemonConfig;

class DaemonRunner
{
public:
    DaemonRunner(const DaemonConfig& config, pid_t launcherPid);
    void Run();

private:
    bool HandleConfigurationMessage(protocol::MessageHeader *pHeader);
    bool HandleLaunchMessage(protocol::MessageHeader *pHeader);
    bool LaunchGame(protocol::LaunchMessage *pMessage);

    pid_t                   m_launcherPid = -1;
    pid_t                   m_gamePid = -1;
    std::vector<LaunchInfo> m_launchInfo;
    const DaemonConfig&     m_config;
};