#include "reader_writer.hpp"

#include <boost/asio.hpp>
#include <iostream>

namespace chat {
	ReaderWriter::ReaderWriter(boost::asio::ip::tcp::socket socket)
		: socket_(std::move(socket))
		, connected_(true)
	{}

	void ReaderWriter::read() {
		while (connected_.load()) {
			// Read message header
			uint32_t length = 0;
			boost::system::error_code error;
			std::cout << "(debug) reading msg header" << std::endl;
			boost::asio::read(socket_, boost::asio::buffer(&length, sizeof(length)), error);
			if (!handle_error(error)) {
				return;
			}
			// TODO: Refactor to have 1 reading and 1 writing at a time. Maybe send tag standalone first to determine next msg?
			// Add message tags: (0) msg_length (1) msg (2) ack_length (3) ack
			// 0: Save msg length in std::atomic<uint32_t> msg_length_
			// 1: 
			// 2: Save ack length in std::atomic<uint32_t> ack_length_

			// Read data based on message length
			std::vector<char> data(length);
			std::cout << "(debug) reading msg " << length << " bytes" << std::endl;
			boost::asio::read(socket_, boost::asio::buffer(data), error);
			if (!handle_error(error)) {
				return;
			}

			const auto client_message = std::string(data.begin(), data.end());
			std::cout << "Received: " << client_message << std::endl;
			length = client_message.size();

			// Send ack header
			std::cout << "(debug) writing ack header" << std::endl;
			boost::asio::write(socket_, boost::asio::buffer(&length, sizeof(length)), error);
			if (!handle_error(error)) {
				return;
			}
			// Send ack message
			std::cout << "(debug) writing ack " << length << " bytes" << std::endl;
			boost::asio::write(socket_, boost::asio::buffer(client_message), error);
			if (!handle_error(error)) {
				return;
			}
			std::cout << "(debug) sent ack" << std::endl;
		}
	}


	void ReaderWriter::write()
	{
		while (connected_.load()) {
			std::string message{};
			boost::system::error_code error;

			// Get user input
			std::cout << "Enter message: \n";
			std::getline(std::cin, message);
			uint32_t length = message.size();

			// Record start time before writing to socket
			const auto start = std::chrono::high_resolution_clock::now();

			// Send header
			std::cout << "(debug) writing msg header" << std::endl;
			boost::asio::write(socket_, boost::asio::buffer(&length, sizeof(length)), error);
			if (!handle_error(error)) {
				return;
			}

			std::vector<char> data(message.begin(), message.end());

			// Send message
			std::cout << "(debug) writing msg " << length << " bytes" << std::endl;
			boost::asio::write(socket_, boost::asio::buffer(data), error);
			if (!handle_error(error)) {
				return;
			}

			std::cout << "(debug) reading ack header" << std::endl;

			// Read ack header
			boost::asio::read(socket_, boost::asio::buffer(&length, sizeof(length)), error);
			if (!handle_error(error)) {
				return;
			}

			std::cout << "(debug) reading ack message " << length << " bytes" << std::endl;

			// Read ack based on byte length
			std::vector<char> response(length);
			boost::asio::read(socket_, boost::asio::buffer(response), error);
			if (!handle_error(error)) {
				return;
			}
			// Record end time after reading response
			const auto end = std::chrono::high_resolution_clock::now();
			const std::chrono::duration<double> round_trip_time = end - start;

			const auto ack_message = std::string(response.begin(), response.end());
			std::cout << "Acknowledgement: " << ack_message << "\n"
				<< "Round trip time (RTT): " << round_trip_time.count() << " seconds" << std::endl;
		}
	}

	bool ReaderWriter::handle_error(boost::system::error_code error) {
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
}