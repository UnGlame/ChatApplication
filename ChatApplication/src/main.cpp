#include "chat_client.hpp"
#include "chat_server.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "<mode>\n"
                  << "mode: 'server' | 'client'" << std::endl;
        return 1;
    }

    const std::string mode = argv[1];
    if (mode != "server" && mode != "client") {
        std::cerr << "Mode is invalid! Use \"server\" or \"client\" as argument." << std::endl;
        return 1;
    }

    std::cout << "ChatApplication is in " << mode << " mode." << std::endl;

    // Run in server mode
    if (mode == "server") {
        chat::Server server{};
        server.run();
        return 0;
    }

    // Run in client mode
    std::cout << "Please input server's IP/hostname for connection:" << std::endl;
    std::string server_ip{};
    std::getline(std::cin, server_ip);
    chat::Client client{};
    client.run(server_ip);

    return 0;
}
