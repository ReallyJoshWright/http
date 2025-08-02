#include <expected>
#include <format>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "logger.hpp"
#include "tcp_async.hpp"

using std::unexpected;
using std::expected;
using std::string;
using std::format;

namespace http {
    TcpAsync::TcpAsync() : log_(false, false) {
        log_to_console_ = false;
        log_to_file_ = false;
        server_socket_ = -1;
        epoll_fd_ = -1;
    }

    TcpAsync::TcpAsync(bool log_to_console) : log_(log_to_console, false) {
        log_to_console_ = log_to_console;
        log_to_file_ = false;
        server_socket_ = -1;
        epoll_fd_ = -1;
    }

    TcpAsync::TcpAsync(bool log_to_console, bool log_to_file) : log_(log_to_console, log_to_file) {
        log_to_console_ = log_to_console;
        log_to_file_ = log_to_file;
        server_socket_ = -1;
        epoll_fd_ = -1;
    }

    TcpAsync::~TcpAsync() {
        closeSocket(server_socket_);
    }

    expected<void, string> TcpAsync::createSocket(int port) {
        int enable = 1;
        sockaddr_in address {};

        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ < 0) {
            log_.error("Socket creation failed");
            return unexpected("Socket creation failed");
        }

        int opt_res = setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&enable, sizeof(enable));
        if (opt_res < 0) {
            log_.error("setsockopt failed");
            closeSocket(server_socket_);
            return unexpected("setsockopt failed");
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        int bind_res = bind(server_socket_, (sockaddr *)&address, sizeof(address));
        if (bind_res < 0) {
            log_.error("Bind failed");
            closeSocket(server_socket_);
            return unexpected("Bind failed");
        }

        int listen_res = listen(server_socket_, SOMAXCONN);
        if (listen_res < 0) {
            log_.error("Listen failed");
            closeSocket(server_socket_);
            return unexpected("Listen failed");
        }

        int non_blocking_res = setNonBlocking(server_socket_);
        if (non_blocking_res < 0) {
            log_.error("Nonblocking failed");
            closeSocket(server_socket_);
            return unexpected("Nonblocking failed");
        }

        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) {
            log_.error("epoll_create1 failed");
            closeSocket(server_socket_);
            return unexpected("epoll_create1 failed");
        }

        event_.events = EPOLLIN;
        event_.data.fd = server_socket_;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_socket_, &event_);

        log_.info(format("Server listening on port {}...", port));

        return {};
    }

    expected<void, string> TcpAsync::acceptClient() {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int res = getsockname(server_socket_, (struct sockaddr *)&addr, &addrlen);
        if (res != 0) {
            log_.error("Error retrieving server address with getsockname()");
            return unexpected("Error retrieving server address with getsockname()");
        }

        int client_socket = accept(server_socket_, (sockaddr *)&addr, &addrlen);
        if (client_socket < 0) {
            log_.error("Accept failed");
            return unexpected("Accept failed");
        }

        log_.info(format("Client socket created: {}", client_socket));

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        log_.info(format(
            "Connection accepted from {}:{}",
            client_ip,
            ntohs(addr.sin_port)
        ));

        return {};
    }

    expected<void, string> TcpAsync::acceptClientWithLoop() {
        while (true) {
            int n = epoll_wait(epoll_fd_, events_, max_events_, -1);
            for (int i = 0; i < n; ++i) {
                if (events_[i].data.fd == server_socket_) {
                    sockaddr_in addr;
                    socklen_t addrlen = sizeof(addr);
                    int res = getsockname(server_socket_, (struct sockaddr *)&addr, &addrlen);
                    if (res != 0) {
                        log_.error("Error retrieving server address with getsockname()");
                        return unexpected("Error retrieving server address with getsockname()");
                    }

                    int client_socket = accept(server_socket_, (sockaddr *)&addr, &addrlen);
                    if (client_socket < 0) {
                        log_.error("Accept failed");
                        return unexpected("Accept failed");
                    }

                    setNonBlocking(client_socket);
                    epoll_event client_event {};
                    client_event.events = EPOLLIN | EPOLLET;
                    client_event.data.fd = client_socket;
                    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_socket, &client_event);

                    log_.info(format("Client socket created: {}", client_socket));
                } else {
                    char buffer[buffer_size_];
                    int bytes = read(events_[i].data.fd, buffer, sizeof(buffer) - 1);
                    if (bytes <= 0) {
                        closeSocket(events_[i].data.fd);
                        log_.info(format("Closed connection: {}", (int)events_[i].data.fd));
                    } else {
                        buffer[bytes] = '\0';
                        log_.info(format("Received from {}: {}", (int)events_[i].data.fd, buffer));
                        send(events_[i].data.fd, buffer, bytes, 0);
                    }
                }
            }
        }

        return {};
    }

    expected<void, string> TcpAsync::acceptClientWithLoopWithMessages() {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int res = getsockname(server_socket_, (struct sockaddr *)&addr, &addrlen);
        if (res != 0) {
            log_.error("Error retrieving server address with getsockname()");
            return unexpected("Error retrieving server address with getsockname()");
        }

        while (true) {
            int client_socket = accept(server_socket_, (sockaddr *)&addr, &addrlen);
            if (client_socket < 0) {
                log_.error("Accept failed");
                return unexpected("Accept failed");
            }

            log_.info(format("Client socket created: {}", client_socket));

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            log_.info(format(
                "Connection accepted from {}:{}",
                client_ip,
                ntohs(addr.sin_port)
            ));

            char buffer[4096] = {0};
            int bytes_received;

            while ((bytes_received = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytes_received] = '\0';
                string message(buffer);
                log_.info(format("{}", message));
            }

            closeSocket(client_socket);
        }

        return {};
    }

    void TcpAsync::closeSocket(int socket) {
        if (socket >= 0) {
            close(socket);
        }
    }

    int TcpAsync::setNonBlocking(int socket) {
        int flags = fcntl(socket, F_GETFL, 0);
        return fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    }
}
