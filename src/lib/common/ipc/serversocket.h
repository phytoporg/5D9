#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace common { namespace ipc {
    class ServerSocket
    {
    public:
        ServerSocket(const std::string& socketName);
        ~ServerSocket();

        bool Bind();
        bool Listen();
        bool Accept(int* pClientFdOut);
        bool Write(int clientFd, uint8_t* pBuffer, size_t bufferSize);
        bool Read(int clientFd, uint8_t* pBuffer, size_t bufferSize);
        void Close(int clientFd);

    private:
        ServerSocket(const ServerSocket& other) = delete;
        ServerSocket() = delete;

        const std::string& m_socketName;
        int                m_fd = -1;
    };
}}