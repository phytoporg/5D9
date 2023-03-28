#pragma once

#include <string>

namespace common { namespace ipc {
    class Socket
    {
    public:
        Socket(const std::string& socketName);

    private:
        Socket(const Socket& other) = delete;
        Socket() = delete;

        const std::string& m_socketName;
    };
}}