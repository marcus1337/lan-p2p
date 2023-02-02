#include "p2p/PeerReceiver.h"
#include <exception>
#include <iostream>
#include <array>

using namespace peer2peer;

void PeerReceiver::addMessage(std::string msg) {
    std::lock_guard<std::mutex> lock(msgMutex);
    messages.push(msg);
}

void PeerReceiver::receive() {
    while (stateWrap.getState() == LinkState::CONNECTED) {
        asio::error_code ec;
        uint8_t data[512];
        size_t len = socket.read_some(asio::buffer(data), ec);
        std::string str(data, data + len);
        
        if (ec && ec != asio::error::eof) {
            std::cout << "error: " << ec.message() << "\n";
            stateWrap.setState(LinkState::DISCONNECTED);
        }
        else {
            addMessage(str);
        }
    }

}

PeerReceiver::PeerReceiver(asio::ip::tcp::socket& _socket, LinkStateWrap& _stateWrap) : socket(_socket), stateWrap(_stateWrap) {
    receiveThread = std::thread(&PeerReceiver::receive, this);
}

PeerReceiver::~PeerReceiver() {
    stateWrap.setState(LinkState::DISCONNECTED);
    socket.close();
    if (receiveThread.joinable())
        receiveThread.join();
}

std::string PeerReceiver::popMessage() {
    std::lock_guard<std::mutex> lock(msgMutex);
    std::string msg = messages.front();
    messages.pop();
    return msg;
}

bool PeerReceiver::hasMessage() {
    std::lock_guard<std::mutex> lock(msgMutex);
    return !messages.empty();
}