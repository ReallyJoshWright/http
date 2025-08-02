#include <expected>
#include <cstring>
#include <vector>
#include <format>
#include <string>
#include <cerrno>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "logger.hpp"
#include "socket.hpp"
#include "tcp.hpp"

using std::unexpected;
using std::expected;
using std::vector;
using std::string;
using std::format;

namespace http {
    ///////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////
    Tcp::Tcp() : log_(false, false) {
        log_to_console_ = false;
        log_to_file_ = false;
    }

    Tcp::Tcp(bool log_to_console) : log_(log_to_console, false) {
        log_to_console_ = log_to_console;
        log_to_file_ = false;
    }

    Tcp::Tcp(bool log_to_console, bool log_to_file)
    : log_(log_to_console, log_to_file) {
        log_to_console_ = log_to_console;
        log_to_file_ = log_to_file;
    }

    ///////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////
    Tcp::~Tcp() noexcept {
    }

    ///////////////////////////////////////////////////////////////////////////
    // public member functions
    ///////////////////////////////////////////////////////////////////////////
    expected<void, string> Tcp::createSocket(int port) {
        int enable = 1;
        sockaddr_in address {};

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
            log_.error("Socket creation failed");
            return unexpected("Socket creation failed");
        }
        server_socket_ = Socket(server_socket);

        int opt_res = setsockopt(
            server_socket_.get(),
            SOL_SOCKET,
            SO_REUSEADDR,
            (const char *)&enable,
            sizeof(enable)
        );

        if (opt_res < 0) {
            log_.error(format("setsockopt failed: {}", strerror(errno)));
            return unexpected(format("setsockopt failed", strerror(errno)));
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        int bind_res = bind(
            server_socket_.get(),
            (sockaddr *)&address,
            sizeof(address)
        );

        if (bind_res < 0) {
            log_.error("Bind failed");
            return unexpected("Bind failed");
        }

        int listen_res = listen(server_socket_.get(), SOMAXCONN);
        if (listen_res < 0) {
            log_.error("Listen failed");
            return unexpected("Listen failed");
        }

        log_.info(format("Server listening on port {}...", port));

        return {};
    }

    expected<void, string> Tcp::acceptClient() {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        Socket client_socket = Socket(accept(
            server_socket_.get(),
            (sockaddr *)&addr,
            &addrlen
        ));

        if (client_socket.get() < 0) {
            log_.error("Accept failed");
            return unexpected("Accept failed");
        }

        log_.info(format("Client socket created: {}", client_socket.get()));

        char client_ip[INET_ADDRSTRLEN];
        const char *inet_ntop_res = inet_ntop(
            AF_INET,
            &(addr.sin_addr),
            client_ip,
            INET_ADDRSTRLEN
        );

        if (inet_ntop_res == nullptr) {
            log_.error("inet_ntop failed");
        }

        log_.info(format(
            "Connection accepted from {}:{}",
            client_ip,
            ntohs(addr.sin_port)
        ));

        return {};
    }

    expected<void, string> Tcp::acceptClientWithLoop() {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        while (true) {
            Socket client_socket = Socket(accept(
                server_socket_.get(),
                (sockaddr *)&addr,
                &addrlen
            ));

            if (client_socket.get() < 0) {
                log_.error("Accept failed");
                return unexpected("Accept failed");
            }

            log_.info(format(
                "Client socket created: {}",
                client_socket.get()
            ));

            char client_ip[INET_ADDRSTRLEN];
            const char *inet_ntop_res = inet_ntop(
                AF_INET,
                &(addr.sin_addr),
                client_ip,
                INET_ADDRSTRLEN
            );

            if (inet_ntop_res == nullptr) {
                log_.error("inet_ntop failed");
            }

            log_.info(format(
                "Connection accepted from {}:{}",
                client_ip,
                ntohs(addr.sin_port)
            ));
        }

        return {};
    }

    expected<void, string> Tcp::acceptClientWithLoopWithMsgs() {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        while (true) {
            Socket client_socket = Socket(accept(
                server_socket_.get(),
                (sockaddr *)&addr,
                &addrlen
            ));

            if (client_socket.get() < 0) {
                log_.error("Accept failed");
                return unexpected("Accept failed");
            }

            log_.info(format(
                "Client socket created: {}",
                client_socket.get()
            ));

            char client_ip[INET_ADDRSTRLEN];
            const char *inet_ntop_res = inet_ntop(
                AF_INET,
                &(addr.sin_addr),
                client_ip,
                INET_ADDRSTRLEN
            );

            if (inet_ntop_res == nullptr) {
                log_.error("inet_ntop failed");
            }

            log_.info(format(
                "Connection accepted from {}:{}",
                client_ip,
                ntohs(addr.sin_port)
            ));

            vector<char> buffer(4096);
            int bytes_received;

            while (true) {
                bytes_received = read(
                    client_socket.get(),
                    buffer.data(),
                    buffer.size() - 1
                );

                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';
                    string message(buffer.data());
                    log_.info(format("{}", message));
                } else if (bytes_received == 0) {
                    log_.info("Client disconnected gracefully");
                    break;
                } else {
                    if (errno == EINTR) {
                        continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        log_.error("No data available to read");
                        continue;
                    } else {
                        log_.error(format(
                            "Error reading from socket: {}",
                            strerror(errno)
                        ));
                        break;
                    }
                }
            }
        }

        return {};
    }
}
