#pragma once

#include <expected>
#include <string>

#include <sys/epoll.h>

#include "logger.hpp"

namespace http {
    class TcpAsync {
        public:
            TcpAsync();
            TcpAsync(bool log_to_console);
            TcpAsync(bool log_to_console, bool log_to_file);
            ~TcpAsync();
            std::expected<void, std::string> createSocket(int port);
            std::expected<void, std::string> acceptClient();
            std::expected<void, std::string> acceptClientWithLoop();
            std::expected<void, std::string> acceptClientWithLoopWithMessages();
    
        private:
            bool log_to_console_;
            bool log_to_file_;
            Logger log_;
            int server_socket_;
            int epoll_fd_;
            constexpr static int max_events_ = 1024;
            constexpr static int buffer_size_ = 4096;
            epoll_event event_ {};
            epoll_event events_[max_events_];

        private:
            void closeSocket(int socket);
            int setNonBlocking(int socket);
    };
}
