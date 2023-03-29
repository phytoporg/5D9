#include "daemonrunner.h"

#include <common/ipc/socket.h>
#include <common/log/log.h>

using namespace common;

DaemonRunner::DaemonRunner(const DaemonConfig& config)
    : m_config(config)
{}

void DaemonRunner::Run()
{
    const int PortNumber = 6500;
    ipc::Socket serverSocket("5D9d", PortNumber);
    if (!serverSocket.Bind())
    {
        return;
    }

    while(true)
    {
        if (serverSocket.Listen())
        {
            RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Listened");
            int clientFd;
            if (serverSocket.Accept(&clientFd))
            {
                RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Accepted");
                uint8_t buffer[256];
                while (serverSocket.Read(clientFd, buffer, sizeof(buffer)))
                {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Testing: %s", buffer);
                }
            }
        }
    }
}
