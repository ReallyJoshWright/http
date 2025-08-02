#include <print>

#include "tcp.hpp"

using std::println;

int main() {
    http::Tcp tcp(true);
    auto create_res = tcp.createSocket(3000);
    if (!create_res.has_value()) {
        println("{}", create_res.error());
    }

    auto accept_res = tcp.acceptClientWithLoopWithMsgs();
    if (!accept_res.has_value()) {
        println("{}", accept_res.error());
    }

    return 0;
}
