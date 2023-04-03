#include "daemonrunner.h"

#include <common/ipc/serversocket.h>
#include <common/log/log.h>

#include <vector>

using namespace common;

namespace {
    bool ReadUntil(ipc::ServerSocket& serverSocket, int clientFd, uint8_t* pData, size_t bytesToRead)
    {
        ssize_t totalBytesRead = serverSocket.Read(clientFd, pData, bytesToRead);
        if (totalBytesRead == bytesToRead)
        {
            return true;
        }
        else if (totalBytesRead < 0)
        {
            return false;
        }

        while (totalBytesRead < bytesToRead)
        {
            const ssize_t BytesRead = serverSocket.Read(clientFd, pData + totalBytesRead, bytesToRead - totalBytesRead);
            if (BytesRead < 0)
            {
                break;
            }

            totalBytesRead += BytesRead;
        }

        return totalBytesRead == bytesToRead;
    }
}

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
                // Read the message header
                const size_t HeaderSize = sizeof(protocol::MessageHeader);
                std::vector<uint8_t> messageBuffer(HeaderSize, 0);

                if (!ReadUntil(serverSocket,
                               clientFd,
                               messageBuffer.data(),
                               HeaderSize))
                {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to read message header");
                    serverSocket.Close(clientFd);
                    continue;
                }

                // Check message header integrity
                auto pMessageHeader = reinterpret_cast<protocol::MessageHeader*>(messageBuffer.data());
                if (pMessageHeader->Magic != protocol::kMessageMagic)
                {
                    RELEASE_LOGLINE_ERROR(
                        LOG_DEFAULT,
                        "Failed to read message header: unexpected magic value");
                    serverSocket.Close(clientFd);
                    continue;
                }

                // Read the remainder of the message
                const size_t MessageLength = pMessageHeader->MessageLen;
                const protocol::MessageType MessageType = pMessageHeader->Type;
                messageBuffer.resize(MessageLength);

                // In case resizing causes a reallocation, update pMessageHeader
                pMessageHeader = reinterpret_cast<protocol::MessageHeader*>(messageBuffer.data());
                if (!ReadUntil(serverSocket,
                               clientFd,
                               messageBuffer.data() + HeaderSize,
                               MessageLength - HeaderSize))
                {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to read message data");
                    serverSocket.Close(clientFd);
                    continue;
                }

                switch (MessageType)
                {
                    case protocol::MessageType::Configure:
                        if (!HandleConfigurationMessage(pMessageHeader))
                        {
                            RELEASE_LOGLINE_ERROR(
                                LOG_DEFAULT,
                                "Failed to handle configuration message.");
                        }
                        break;
                    case protocol::MessageType::Launch:
                        if (!HandleLaunchMessage(pMessageHeader))
                        {
                            RELEASE_LOGLINE_ERROR(
                                LOG_DEFAULT,
                                "Failed to handle launch message.");
                        }
                        break;
                    default:
                        RELEASE_LOGLINE_ERROR(
                            LOG_DEFAULT,
                            "Received unexpected message type: %d",
                            pMessageHeader->Type);
                        break;
                }
            }
            serverSocket.Close(clientFd);
            RELEASE_LOGLINE_INFO(LOG_DEFAULT, "Socket closed for client handle %d", clientFd);
        }
    }
}

bool DaemonRunner::HandleConfigurationMessage(protocol::MessageHeader *pHeader)
{
    RELEASE_LOGLINE_INFO(LOG_DEFAULT, "Received Configuration message.");

    auto pConfigureMessage = reinterpret_cast<protocol::ConfigureMessage*>(pHeader);
    const uint8_t Count = pConfigureMessage->ConfigurationCount;
    RELEASE_LOGLINE_INFO(LOG_DEFAULT, "%d configurations", Count);
    for (uint8_t i = 0; i < Count; ++i)
    {
        const protocol::GameConfiguration& Configuration = pConfigureMessage->Configurations[i];
        const LaunchInfo Info {
            .Name = Configuration.Name,
            .Command = Configuration.Command
        };

        // No duplicate entries
        auto it = std::find_if(std::begin(m_launchInfo), std::end(m_launchInfo),
       [&Info](const LaunchInfo& infoEntry) -> bool
            {
                return Info.Name == infoEntry.Name;
            });
        if (it != std::end(m_launchInfo))
        {
            RELEASE_LOGLINE_WARNING(
                LOG_DEFAULT,
                "Launch info already exists for process with name %s. Skipping.",
                Info.Name.c_str());
        }
        else
        {
            m_launchInfo.push_back(Info);
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Added launch info: %s, %s",
                Info.Name.c_str(),
                Info.Command.c_str());
        }
    }

    return true;
}

bool DaemonRunner::HandleLaunchMessage(protocol::MessageHeader *pHeader)
{
    // TODO
    return false;
}

