#include "chat_client.hpp"

#include "reader_writer.hpp"

#include <boost/asio.hpp>

#include <iostream>

namespace chat {
using boost::asio::ip::tcp;

void Client::run(std::string_view server_ip)
{
    try {
        boost::asio::io_context context;
        tcp::resolver resolver(context);
        tcp::socket socket(context);

        // Connect to given IP address
        auto endpoints = resolver.resolve(server_ip, std::to_string(constants::port));
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server.\n"
                  << "To send a chat message, type your message and press enter. "
                  << "The client will receive the message and send an "
                     "acknowledgement back to you (the client). "
                  << "You will receive the ack and the Round-Trip Time (RTT) will "
                     "be displayed. "
                  << "Likewise, if you receive a chat message, you (the client) "
                     "will send an acknowledgement back to the server."
                  << std::endl;

        ReaderWriter reader_writer{std::move(socket), context};
        std::thread reader_thread(&ReaderWriter::read, &reader_writer);
        std::thread writer_thread(&ReaderWriter::write, &reader_writer);

        reader_thread.join();
        writer_thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
} // namespace chat
