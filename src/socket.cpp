#include <sys/socket.h>
#include <unistd.h>

#include "socket.hpp"

namespace http {
    ///////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////
    Socket::Socket() {
        fd_ = -1;
    }

    Socket::Socket(int fd) {
        fd_ = fd;
    }

    Socket::Socket(Socket &&other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }

    Socket& Socket::operator=(Socket &&other) noexcept {
        if (this != &other) {
            close();
            fd_ = other.fd_;
            other.fd_ = -1;
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////
    Socket::~Socket() noexcept {
        close();
    }

    ///////////////////////////////////////////////////////////////////////////
    // public member functions
    ///////////////////////////////////////////////////////////////////////////
    int Socket::get() const {
        return fd_;
    }

    int Socket::release() {
        int old = fd_;
        fd_ = -1;
        return old;
    }

    ///////////////////////////////////////////////////////////////////////////
    // private member functions
    ///////////////////////////////////////////////////////////////////////////
    void Socket::close() noexcept {
        if (fd_ >= 0) {
            ::shutdown(fd_, SHUT_RDWR);
            ::close(fd_);
        }
    }
}
