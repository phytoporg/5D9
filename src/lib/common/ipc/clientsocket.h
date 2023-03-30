#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace common { namespace ipc {
    class ClientSocket
    {
    public:
        ClientSocket(const std::string &socketName);
        ~ClientSocket();

        bool Connect();
        bool Write(uint8_t* pBuffer, size_t bufferSize);
        bool Read(uint8_t* pBuffer, size_t bufferSize);
        void Close();

    private:
        ClientSocket(const ClientSocket& other) = delete;
        ClientSocket() = delete;

        const std::string& m_socketName;
        int                m_fd = -1;
    };
}}
