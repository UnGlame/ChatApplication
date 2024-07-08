#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>

namespace constants {
static constexpr uint16_t port = 12345;
static constexpr auto header_size = sizeof(uint32_t);
} // namespace constants

namespace chat {

class ReaderWriter
{
public:
    ReaderWriter(boost::asio::ip::tcp::socket socket, boost::asio::io_context& io_context);
    ~ReaderWriter() = default;
    ReaderWriter(const ReaderWriter&) = delete;
    ReaderWriter(ReaderWriter&&) = delete;
    ReaderWriter& operator=(const ReaderWriter&) = delete;
    ReaderWriter& operator=(ReaderWriter&&) = delete;
    void read();
    void write();

private:
    bool handle_error(boost::system::error_code error);
    void read_input();
    void try_write_ack();
    void try_write_msg();

    boost::asio::ip::tcp::socket socket_;
    std::mutex input_mutex_;
    std::mutex ack_mutex_;
    std::optional<std::string> input_msg_{};
    std::optional<std::string> ack_msg_{};
    std::atomic<bool> connected_;
    std::chrono::time_point<std::chrono::system_clock> rtt_start_{};
};
} // namespace chat
