#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>

using namespace common;
using namespace common::ipc;

Socket::Socket(const std::string& socketName)
    : m_socketName(socketName)
{}