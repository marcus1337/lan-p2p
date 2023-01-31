#include "PeerConnection.h"
#include <exception>
#include <iostream>

using namespace peer2peer;

PeerConnection::PeerConnection(asio::ip::tcp::socket _socket) : socket(std::move(_socket)), receiver(socket) {

}
void PeerConnection::sendMessage(std::string msg) {
    try {
        asio::error_code ignored_error;
        asio::write(socket, asio::buffer(msg), ignored_error);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

LinkState PeerConnection::getState() {
    return LinkStateWrap::getSocketState(socket);
}

void PeerConnection::close() {
    socket.close();
}

