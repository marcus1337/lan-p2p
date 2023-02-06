#ifndef HANDSHAKE_H
#define HANDSHAKE_H
#include "LinkState.h"
#include <asio.hpp>
#include <future>
#include <chrono>

namespace peer2peer {
    class Handshake {

        asio::ip::tcp::socket& socket;
        std::future<bool> makeHandshakeFuture();

        bool sendHELLO();
        bool receiveHELLO();

    public:
        Handshake(asio::ip::tcp::socket& _socket);
        bool operator()();
    };
}

#endif // !HANDSHAKE_H
