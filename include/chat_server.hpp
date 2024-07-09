#pragma once
#include <boost/asio/ip/tcp.hpp>

namespace chat {

class Server
{
public:
    Server() = default;
    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

    void run();
};
} // namespace chat
