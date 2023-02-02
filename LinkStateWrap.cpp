
#include "LinkState.h"
#include <exception>
#include <iostream>

using namespace peer2peer;


LinkStateWrap::LinkStateWrap() : state(LinkState::DISCONNECTED) {

}
LinkState LinkStateWrap::getState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return state;
}
void LinkStateWrap::setState(LinkState _state) {
    std::lock_guard<std::mutex> lock(stateMutex);
    state = _state;
}
bool LinkStateWrap::trySetStateConnected() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (state == LinkState::LOCATING) {
        state = LinkState::CONNECTED;
        return true;
    }
    return false;
}

void LinkStateWrap::setDisconnectedIfLocating() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (state == LinkState::LOCATING) {
        state = LinkState::DISCONNECTED;
    }
}

LinkState LinkStateWrap::getSocketState(asio::ip::tcp::socket& _socket) {
    try {
        if (_socket.is_open() && _socket.remote_endpoint().address() != asio::ip::address()) {
            return LinkState::CONNECTED;
        }
    }
    catch (std::exception& e) {
        std::cout << "getSocketState() exception [" << e.what() << "]\n";
    }
    return LinkState::DISCONNECTED;
}

