#include "p2p/PeerConnection.h"
#include <exception>
#include <iostream>

using namespace peer2peer;

PeerConnection::PeerConnection(asio::ip::tcp::socket&& _socket, LinkStateWrap& _stateWrap) : socket(_socket), receiver(_socket, _stateWrap), stateWrap(_stateWrap) {

}

PeerConnection::~PeerConnection() {
    std::cout << "PeerConnection::close()\n";
    socket.close();
    stateWrap.setState(LinkState::DISCONNECTED);
}

void PeerConnection::sendBytes(std::vector<uint8_t> bytes) {

    uint32_t numBytes = bytes.size();
    asio::error_code ec;
    asio::write(socket, asio::buffer(&numBytes, sizeof(numBytes)), ec);
    if (ec) {
        std::cout << "send error: " << ec.message() << "\n";
        stateWrap.setState(LinkState::DISCONNECTED);
    }
    asio::write(socket, asio::buffer(bytes), ec);
    if (ec) {
        std::cout << "send error: " << ec.message() << "\n";
        stateWrap.setState(LinkState::DISCONNECTED);
    }
}



