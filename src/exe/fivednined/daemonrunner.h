#pragma once

#include "launchinfo.h"
#include "protocol/message.h"
#include <vector>

class DaemonConfig;

class DaemonRunner
{
public:
    DaemonRunner(const DaemonConfig& config);
    void Run();

private:
    bool HandleConfigurationMessage(protocol::MessageHeader *pHeader);
    bool HandleLaunchMessage(protocol::MessageHeader *pHeader);

    std::vector<LaunchInfo> m_launchInfo;
    const DaemonConfig& m_config;
};