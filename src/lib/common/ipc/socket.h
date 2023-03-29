#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace common { namespace ipc {
    class Socket
    {
    public:
        Socket(const std::string& socketName, const int portNumber);
        ~Socket();

        bool Bind();
        bool Listen();
        bool Accept(int* pClientFdOut);
        bool Write(int clientFd, uint8_t* pBuffer, size_t bufferSize);
        bool Read(int clientFd, uint8_t* pBuffer, size_t bufferSize);
        void Close(int clientFd);

    private:
        Socket(const Socket& other) = delete;
        Socket() = delete;

        const std::string& m_socketName;
        int                m_portNumber = -1;
        int                m_fd = -1;
    };
}}