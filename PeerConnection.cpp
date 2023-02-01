#include "PeerConnection.h"
#include <exception>
#include <iostream>

using namespace peer2peer;

PeerConnection::PeerConnection(asio::ip::tcp::socket& _socket) : socket(_socket), receiver(_socket, socketMutex, stateWrap), stateWrap(LinkState::CONNECTED) {

}

void PeerConnection::sendMessage(std::string msg) {
    std::lock_guard<std::mutex> loc(socketMutex);
    asio::error_code ec;
    socket.write_some(asio::buffer(msg), ec);
    if (ec) {
        std::cout << "send error: " << ec.message() << "\n";
        stateWrap.setState(LinkState::DISCONNECTED);
    }
}

LinkState PeerConnection::getState() {
    return stateWrap.getState();
}

void PeerConnection::close() {
    std::cout << "PeerConnection::close()\n";
    socket.close();
}

