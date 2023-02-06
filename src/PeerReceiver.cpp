#include "p2p/PeerReceiver.h"
#include <exception>
#include <iostream>
#include <array>

using namespace peer2peer;

void PeerReceiver::addBytes(std::vector<uint8_t> msg) {
    std::lock_guard<std::mutex> lock(msgMutex);
    if(messages.size() < MAX_STORED_MESSAGES)
        messages.push(msg);
}

void PeerReceiver::receive() {
    while (stateWrap.getState() == LinkState::CONNECTED) {
        asio::error_code ec;
        uint8_t data[MAX_MESSAGE_SIZE];

        uint32_t numBytes;
        asio::read(socket, asio::buffer(&numBytes, sizeof(numBytes)), ec);
        if ((ec && ec != asio::error::eof) || numBytes >= MAX_MESSAGE_SIZE) {
            std::cout << "error: " << ec.message() << " numBytes " << numBytes << "\n";
            stateWrap.setState(LinkState::DISCONNECTED);
            return;
        }

        asio::read(socket, asio::buffer(&data, numBytes), ec);

        if (ec && ec != asio::error::eof) {
            std::cout << "error: " << ec.message() << " numBytes " << numBytes << "\n";
            stateWrap.setState(LinkState::DISCONNECTED);
        }
        else {
            std::vector<uint8_t> bytes(data, data + numBytes);
            addBytes(bytes);
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

std::vector<uint8_t> PeerReceiver::popBytes() {
    std::lock_guard<std::mutex> lock(msgMutex);
    auto msg = messages.front();
    messages.pop();
    return msg;
}

bool PeerReceiver::hasReceivedData() {
    std::lock_guard<std::mutex> lock(msgMutex);
    return !messages.empty();
}