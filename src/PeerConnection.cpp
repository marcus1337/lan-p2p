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

void PeerConnection::sendMessage(std::string msg) {
    asio::error_code ec;
    socket.write_some(asio::buffer(msg), ec);
    if (ec) {
        std::cout << "send error: " << ec.message() << "\n";
        stateWrap.setState(LinkState::DISCONNECTED);
    }
}



