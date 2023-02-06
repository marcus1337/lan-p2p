#include "p2p/Handshake.h"
#include <array>
#include <iostream>
#include <algorithm>

using namespace peer2peer;


Handshake::Handshake(asio::ip::tcp::socket& _socket) : socket(_socket) {

}

bool Handshake::sendHELLO() {
    const std::string handshake_message = "HELLO";
    asio::error_code ec;
    asio::write(socket, asio::buffer(handshake_message), ec);
    if (ec) {
        std::cout << "Handshake HELLO failed.\n";
        return false;
    }
    return true;
}
bool Handshake::receiveHELLO() {
    std::array<char, 5> response;
    asio::error_code ec;
    asio::read(socket, asio::buffer(response), ec);
    if (ec)
    {
        std::cout << "Handshake read HELLO failed.\n";
        return false;
    }
    return std::equal(response.begin(), response.end(), "HELLO");
}

std::future<bool> Handshake::makeHandshakeFuture() {
    return std::async(std::launch::async, [&]() {
        return sendHELLO() && receiveHELLO();
        });
}

bool Handshake::operator()() {
    std::future<bool> future = makeHandshakeFuture();
    std::future_status status = future.wait_for(std::chrono::seconds(4));
    if (status == std::future_status::ready)
        return future.get();
    else {
        std::cout << "Error: handshake timed out.\n";
        socket.close();
        future.wait();
    }
    return false;
}

