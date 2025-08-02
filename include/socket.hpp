#pragma once

namespace http {
    class Socket {
        public:
            Socket();
            explicit Socket(int fd);
            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;
            Socket(Socket &&other) noexcept;
            Socket& operator=(Socket &&other) noexcept;
            ~Socket() noexcept;
            int get() const;
            int release();

        private:
            int fd_;

        private:
            void close() noexcept;
    };
}
