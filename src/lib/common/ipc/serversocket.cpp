#include "serversocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <common/log/check.h>

#include <cstring>

using namespace common;
using namespace common::ipc;

ServerSocket::ServerSocket(const std::string& socketName)
    : m_socketName(socketName)
{
    m_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    RELEASE_CHECK(m_fd > 0, "Failed to create socket for %s", socketName.c_str());
}
ServerSocket::~ServerSocket()
{
    close(m_fd);
    m_fd = -1;
}

bool ServerSocket::Bind()
{
    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sun_family = AF_LOCAL;
    strncpy(saddr.sun_path, m_socketName.c_str(), sizeof(saddr.sun_path));

    if (bind(m_fd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr)) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to bind socket %s", m_socketName.c_str());
        return false;
    }

    return true;
}

bool ServerSocket::Listen()
{
    const int MaxConnections = 1;
    if (listen(m_fd, MaxConnections) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to listen on port %d", m_portNumber);
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "ServerSocket %s listening on port %d", m_socketName.c_str(), m_portNumber);
    return true;
}

bool ServerSocket::Accept(int* pClientFdOut)
{
    struct sockaddr_un caddr;
    int len = sizeof(caddr);

    int clientFd = accept(m_fd, reinterpret_cast<struct sockaddr*>(&caddr), reinterpret_cast<socklen_t*>(&len));
    if (clientFd < 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Failed to accept connection on socket %s port %d",
            m_socketName.c_str(),
            m_portNumber);
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "Accepted connection on socket %s port %d",
        m_socketName.c_str(),
        m_portNumber);

    *pClientFdOut = clientFd;
    return true;
}

bool ServerSocket::Write(int clientFd, uint8_t *pBuffer, size_t bufferSize)
{
    ssize_t numBytesWritten = write(clientFd, pBuffer, bufferSize);
    if (numBytesWritten < 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Failed to write data of size %u for client %d on socket %s",
            bufferSize,
            clientFd,
            m_socketName.c_str());
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "Wrote %d bytes for client %d on socket %s",
        numBytesWritten,
        clientFd,
        m_socketName.c_str());
    return true;
}

bool ServerSocket::Read(int clientFd, uint8_t *pBuffer, size_t bufferSize)
{
    ssize_t numBytesRead = read(clientFd, pBuffer, bufferSize);
    if (numBytesRead <= 0)
    {
        RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Failed to read data of size %u for client %d on socket %s",
                bufferSize,
                clientFd,
                m_socketName.c_str());
        return false;
    }

    RELEASE_LOGLINE_INFO(
            LOG_DEFAULT,
            "Read %d bytes for client %d on socket %s",
            numBytesRead,
            clientFd,
            m_socketName.c_str());
    return true;
}

void ServerSocket::Close(int clientFd)
{
    close(clientFd);
}
