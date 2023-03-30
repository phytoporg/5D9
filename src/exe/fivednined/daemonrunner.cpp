#include "daemonrunner.h"

#include <common/ipc/serversocket.h>
#include <common/log/log.h>

using namespace common;

DaemonRunner::DaemonRunner(const DaemonConfig& config)
    : m_config(config)
{}

void DaemonRunner::Run()
{
    ipc::ServerSocket serverSocket("/tmp/5D9d");
    if (!serverSocket.Bind())
    {
        return;
    }

    while(true)
    {
        if (serverSocket.Listen())
        {
            int clientFd;
            if (serverSocket.Accept(&clientFd))
            {
                uint8_t buffer[256];
                while (serverSocket.Read(clientFd, buffer, sizeof(buffer)))
                {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Testing: %s", buffer);
                }
            }
            serverSocket.Close(clientFd);
        }
    }

}
