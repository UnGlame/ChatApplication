#include "reader_writer.hpp"

#include <boost/asio.hpp>

#include <iostream>

namespace chat {
enum class Tag : uint8_t
{
    ack,
    text
};

ReaderWriter::ReaderWriter(boost::asio::ip::tcp::socket socket, boost::asio::io_context& io_context)
    : socket_{std::move(socket)}
    , connected_{true}
{}

void ReaderWriter::read_input()
{
    std::string input;
    while (connected_.load()) {
        std::getline(std::cin, input);
        {
            std::lock_guard lock{input_mutex_};
            input_msgs_.push(input);
        }
    }
}

void ReaderWriter::read()
{
    while (connected_.load()) {
        uint8_t tag_buffer = 0;
        uint32_t length = 0;
        boost::system::error_code ec;

        // Read tag to identify message type
        boost::asio::read(socket_, boost::asio::buffer(&tag_buffer, sizeof(tag_buffer)), ec);
        if (!handle_error(ec)) {
            return;
        }
        const auto tag = static_cast<Tag>(tag_buffer);

        // Read data length
        boost::asio::read(socket_, boost::asio::buffer(&length, sizeof(length)), ec);
        if (!handle_error(ec)) {
            return;
        }
        // Read data based on length
        std::vector<char> data(length);
        boost::asio::read(socket_, boost::asio::buffer(data), ec);
        if (!handle_error(ec)) {
            return;
        }
        const auto data_str = std::string(data.begin(), data.end());

        switch (tag) {
            case Tag::ack: {
                // Record end time after reading response
                const auto end = std::chrono::high_resolution_clock::now();
                const std::chrono::duration<double> round_trip_time = end - rtt_start_;
                std::cout << "Acknowledgement: " << data_str << "\n"
                          << "Round trip time (RTT): " << round_trip_time.count() << " seconds" << std::endl;
                break;
            }
            case Tag::text: {
                std::cout << "Received text: " << data_str << std::endl;
                std::lock_guard lock(ack_mutex_);
                ack_msgs_.push(data_str);
                break;
            }
            default:
                throw std::runtime_error("Data tag not supported!");
        }
    }
}

void ReaderWriter::write()
{
    while (connected_.load()) {
        try_write_ack();
        try_write_msg();
    }
}

void ReaderWriter::try_write_ack()
{
    std::lock_guard lock(ack_mutex_);
    if (ack_msgs_.empty()) {
        return;
    }
    boost::system::error_code ec;
    const auto tag = static_cast<uint8_t>(Tag::ack);
    const auto ack_msg = ack_msgs_.front();
    ack_msgs_.pop();
    uint32_t length = ack_msg.size();

    // Send ack tag
    boost::asio::write(socket_, boost::asio::buffer(&tag, sizeof(tag)), ec);
    if (!handle_error(ec)) {
        return;
    }
    // Send ack header
    boost::asio::write(socket_, boost::asio::buffer(&length, sizeof(length)), ec);
    if (!handle_error(ec)) {
        return;
    }
    // Send ack message
    boost::asio::write(socket_, boost::asio::buffer(ack_msg), ec);
    if (!handle_error(ec)) {
        return;
    }
}

void ReaderWriter::try_write_msg()
{
    std::lock_guard lock{input_mutex_};
    if (input_msgs_.empty()) {
        return;
    }
    boost::system::error_code ec;
    const auto tag = static_cast<uint8_t>(Tag::text);
    const auto msg = input_msgs_.front();
    input_msgs_.pop();
    uint32_t length = msg.size();
    std::vector<char> data(msg.begin(), msg.end());

    // Record start time before writing to socket
    rtt_start_ = std::chrono::high_resolution_clock::now();

    // Send msg tag
    boost::asio::write(socket_, boost::asio::buffer(&tag, sizeof(tag)), ec);
    if (!handle_error(ec)) {
        return;
    }
    // Send msg length
    boost::asio::write(socket_, boost::asio::buffer(&length, sizeof(length)), ec);
    if (!handle_error(ec)) {
        return;
    }
    // Send message
    boost::asio::write(socket_, boost::asio::buffer(data), ec);
    if (!handle_error(ec)) {
        return;
    }
}

bool ReaderWriter::handle_error(boost::system::error_code error)
{
    if (error == boost::asio::error::eof) {
        // Connected closed cleanly by peer
        connected_.store(false);
        std::cout << "Disconnected from peer." << std::endl;
        return false;
    }
    if (error) {
        std::cerr << "Error: " << error.message() << std::endl;
        connected_.store(false);
        return false;
    }
    return true;
}
} // namespace chat
