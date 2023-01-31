#include "PeerReceiver.h"
#include <exception>
#include <iostream>
#include <array>

using namespace peer2peer;

void PeerReceiver::addMessage(std::string msg) {
    std::lock_guard<std::mutex> lock(msgMutex);
    messages.push_back(msg);
}

void PeerReceiver::receive() {
    while (true) {
        try {
            std::string result;
            asio::read_until(socket, asio::dynamic_buffer(result), "\n");
            std::cout << "receive() [" << result << "]\n";
            addMessage(result);
        }
        catch (std::exception& e) {
            std::cout << "Receiver error: " << e.what() << "\n";
            return;
        }

    }
}

PeerReceiver::PeerReceiver(asio::ip::tcp::socket& _socket) : socket(_socket) {
    receiveThread = std::thread(&PeerReceiver::receive, this);
}

PeerReceiver::~PeerReceiver() {
    if (receiveThread.joinable())
        receiveThread.join();
}

std::vector<std::string> PeerReceiver::popMessages() {
    std::lock_guard<std::mutex> lock(msgMutex);
    std::vector<std::string> messagesCopy = messages;
    messages.clear();
    return messagesCopy;
}

bool PeerReceiver::hasMessage() {
    std::lock_guard<std::mutex> lock(msgMutex);
    return !messages.empty();
}