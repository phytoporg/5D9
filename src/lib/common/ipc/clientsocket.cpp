#include "clientsocket.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include <common/log/check.h>

using namespace common;
using namespace common::ipc;

ClientSocket::ClientSocket(const std::string &socketName)
    : m_socketName(socketName)
{
    m_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    RELEASE_CHECK(m_fd > 0, "Failed to create client socket for %s", socketName.c_str());
}

ClientSocket::~ClientSocket()
{
    Close();
}

bool ClientSocket::Connect()
{
    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sun_family = AF_LOCAL;
    strncpy(saddr.sun_path, m_socketName.c_str(), m_socketName.length());

    if (connect(m_fd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr)) < 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Failed to connect to socket %s: %s",
            m_socketName.c_str(),
            strerror(errno));
        return false;
    }

    return true;
}

bool ClientSocket::Write(uint8_t* pBuffer, size_t bufferSize)
{
    ssize_t numBytesWritten = write(m_fd, pBuffer, bufferSize);
    if (numBytesWritten < 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Failed to write data of size %u on socket %s",
            bufferSize,
            m_socketName.c_str());
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "Wrote %d bytes on socket %s",
        numBytesWritten,
        m_socketName.c_str());
    return true;
}

bool ClientSocket::Read(uint8_t* pBuffer, size_t bufferSize)
{
    ssize_t numBytesRead = read(m_fd, pBuffer, bufferSize);
    if (numBytesRead <= 0)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Failed to read data of size %u on socket %s",
            bufferSize,
            m_socketName.c_str());
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_DEFAULT,
        "Read %d bytes on socket %s",
        numBytesRead,
        m_socketName.c_str());
    return true;
}

void ClientSocket::Close()
{
    close(m_fd);
    m_fd = -1;
}

