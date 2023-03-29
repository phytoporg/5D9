#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <common/log/check.h>

#include <cstring>

using namespace common;
using namespace common::ipc;

Socket::Socket(const std::string& socketName, const int portNumber)
    : m_socketName(socketName), m_portNumber(portNumber)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    RELEASE_CHECK(m_fd > 0, "Failed to create socket for %s", socketName.c_str());
}
Socket::~Socket()
{
    close(m_fd);
    m_fd = -1;
}

bool Socket::Bind()
{
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET; // Should this be AF_LOCAL?
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(m_portNumber);

    if (bind(m_fd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr)) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to bind socket %s", m_socketName.c_str());
        return false;
    }

    return true;
}

bool Socket::Listen()
{
    const int MaxConnections = 1;
    if (listen(m_fd, MaxConnections) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to listen on port %d", m_portNumber);
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "Socket %s listening on port %d", m_socketName.c_str(), m_portNumber);
    return true;
}

bool Socket::Accept(int* pClientFdOut)
{
    struct sockaddr_in caddr;
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

bool Socket::Write(int clientFd, uint8_t *pBuffer, size_t bufferSize)
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

bool Socket::Read(int clientFd, uint8_t *pBuffer, size_t bufferSize)
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

void Socket::Close(int clientFd)
{
    close(clientFd);
}

