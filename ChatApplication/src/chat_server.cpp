#include "chat_server.hpp"
#include "reader_writer.hpp"

#include <boost/asio.hpp>

#include <iostream>


namespace chat {
	using boost::asio::ip::tcp;

	void Server::run() {
		try {
			boost::asio::io_context context;
			tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), constants::port));

			std::cout << "Server is now active." << std::endl;
			constexpr bool active = true;

			// Actively search and maintain connection with client
			while (active) {
				tcp::socket socket(context);
				acceptor.accept(socket);

				std::cout << "Connected to client." << std::endl;

				// Spawn threads to read-write messages to/from client
				ReaderWriter reader_writer{ std::move(socket) };
				std::thread reader_thread(&ReaderWriter::read, &reader_writer);
				std::thread writer_thread(&ReaderWriter::write, &reader_writer);

				reader_thread.join();
				writer_thread.join();
			}
		}
		catch (std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}
}