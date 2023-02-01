#include "PeerReceiver.h"
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
        socketMutex.lock();
        size_t len = socket.read_some(asio::buffer(data), ec);
        socketMutex.unlock();
        std::string str(data, data + len);
        
        if (ec) {
            std::cout << "error: " << ec.message() << "\n";
            stateWrap.setState(LinkState::DISCONNECTED);
        }
        else {
            addMessage(str);
        }
    }

}

PeerReceiver::PeerReceiver(asio::ip::tcp::socket& _socket, std::mutex& _socketMutex, LinkStateWrap& _stateWrap) : socket(_socket), socketMutex(_socketMutex), stateWrap(_stateWrap) {
    receiveThread = std::thread(&PeerReceiver::receive, this);
}

PeerReceiver::~PeerReceiver() {
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