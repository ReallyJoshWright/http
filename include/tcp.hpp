#pragma once

#include <expected>
#include <string>

#include "logger.hpp"
#include "socket.hpp"

namespace http {
    class Tcp {
        public:
            Tcp();
            Tcp(bool log_to_console);
            Tcp(bool log_to_console, bool log_to_file);
            ~Tcp() noexcept;
            std::expected<void, std::string> createSocket(int port);
            std::expected<void, std::string> acceptClient();
            std::expected<void, std::string> acceptClientWithLoop();
            std::expected<void, std::string> acceptClientWithLoopWithMsgs();
    
        private:
            bool log_to_console_;
            bool log_to_file_;
            Logger log_;
            Socket server_socket_;
    };
}
