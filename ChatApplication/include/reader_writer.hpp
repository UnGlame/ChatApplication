#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <atomic>

namespace constants {
	static constexpr uint16_t port = 12345;
}

namespace chat {

	class ReaderWriter {
	public:
		ReaderWriter(boost::asio::ip::tcp::socket socket);
		void read();
		void write();

	private:
		bool handle_error(boost::system::error_code error);

		boost::asio::ip::tcp::socket socket_;
		std::atomic<bool> connected_;
	};
}