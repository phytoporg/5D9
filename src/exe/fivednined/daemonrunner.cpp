#include "daemonrunner.h"

#include <common/ipc/serversocket.h>
#include <common/log/log.h>

#include <vector>
#include <cstring>

#include <unistd.h>

using namespace common;

namespace {
    bool ReadUntil(ipc::ServerSocket& serverSocket, int clientFd, uint8_t* pData, size_t bytesToRead)
    {
        ssize_t totalBytesRead = serverSocket.Read(clientFd, pData, bytesToRead);
        if (totalBytesRead == bytesToRead)
        {
            return true;
        }
        else if (totalBytesRead <= 0)
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

    while(serverSocket.Listen())
    {
        int clientFd;
        if (serverSocket.Accept(&clientFd))
        {
            while (true)
            {
                const size_t HeaderSize = sizeof(protocol::MessageHeader);
                std::vector<uint8_t> messageBuffer(HeaderSize, 0);

                // Read message header
                if (!ReadUntil(serverSocket,
                               clientFd,
                               messageBuffer.data(),
                               HeaderSize)) {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to read message header");
                    serverSocket.Close(clientFd);
                    break;
                }

                // Check message header integrity
                auto pMessageHeader = reinterpret_cast<protocol::MessageHeader *>(messageBuffer.data());
                if (pMessageHeader->Magic != protocol::kMessageMagic) {
                    RELEASE_LOGLINE_ERROR(
                        LOG_DEFAULT,
                        "Failed to read message header: unexpected magic value");
                    serverSocket.Close(clientFd);
                    break;
                }

                // Read the remainder of the message
                const size_t MessageLength = pMessageHeader->MessageLen;
                const protocol::MessageType MessageType = pMessageHeader->Type;
                messageBuffer.resize(MessageLength);

                // In case resizing causes a reallocation, update pMessageHeader
                pMessageHeader = reinterpret_cast<protocol::MessageHeader *>(messageBuffer.data());
                if (!ReadUntil(serverSocket,
                               clientFd,
                               messageBuffer.data() + HeaderSize,
                               MessageLength - HeaderSize)) {
                    RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to read message data");
                    serverSocket.Close(clientFd);
                    break;
                }

                switch (MessageType)
                {
                    case protocol::MessageType::Configure:
                        if (!HandleConfigurationMessage(pMessageHeader)) {
                            RELEASE_LOGLINE_ERROR(
                                LOG_DEFAULT,
                                "Failed to handle configuration message.");
                        }
                        break;
                    case protocol::MessageType::Launch:
                        if (!HandleLaunchMessage(pMessageHeader)) {
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
        }

        serverSocket.Close(clientFd);
        RELEASE_LOGLINE_INFO(LOG_DEFAULT, "Socket closed for client handle %d", clientFd);
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
    RELEASE_LOGLINE_INFO(LOG_DEFAULT, "Received Launch message.");

    auto pLaunchMessage = reinterpret_cast<protocol::LaunchMessage*>(pHeader);
    if (!LaunchGame(pLaunchMessage))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT, "Failed to launch process: %s",
            pLaunchMessage->Name);
        return false;
    }

    return true;
}

bool DaemonRunner::LaunchGame(protocol::LaunchMessage *pMessage)
{
    auto it =
    std::find_if(std::begin(m_launchInfo), std::end(m_launchInfo),
    [pMessage](const LaunchInfo& info) -> bool {
        return info.Name == pMessage->Name;
    });
    if (it == std::end(m_launchInfo))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Could not find matching configuration for %s",
            pMessage->Name);
        return false;
    }

    const char* pCommand = it->Command.c_str();
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Fork failed when launching %s",
            pMessage->Name);
        return false;
    }
    else if (pid > 0)
    {
        // Parent process
        // TODO: Store the pid somewhere
    }
    else
    {
        std::vector<char*> tokens;
        char* token = std::strtok(const_cast<char*>(pCommand), " ");
        while (token)
        {
            tokens.push_back(token);
            token = std::strtok(nullptr, " ");
        }

        if (tokens.empty())
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT, "Cannot launch %s: command is empty",
                pMessage->Name);
            return false;
        }
        tokens.push_back(nullptr);

        if (execv(tokens[0], &tokens[0]) < 0)
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT, "Failed to execute command: %s",
                pCommand);
            exit(-1);
        }
    }

    return true;
}
