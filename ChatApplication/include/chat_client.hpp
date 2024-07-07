#pragma once
#include <string_view>

namespace chat {
	class Client
	{
	public:
		Client() = default;
		Client(const Client&) = delete;
		Client(Client&&) = delete;
		Client& operator=(Client&) = delete;
		Client& operator=(Client&&) = delete;

		void run(std::string_view server_ip);
	};
}