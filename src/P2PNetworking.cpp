#include "P2PNetworking.h"
#include <iostream>
#include <chrono>

using namespace peer2peer;

P2PNetworking::P2PNetworking() : peerDiscovery(stateWrap) {

}

P2PNetworking::~P2PNetworking() {
    stop();
}

void P2PNetworking::start() {
    startTimeInMilliseconds = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    stop();
    peerDiscovery.start();
}

void P2PNetworking::stop() {
    stateWrap.setState(LinkState::DISCONNECTED);
    peerDiscovery.stop();
    if (connection != nullptr) {
        connection->stop();
    }
    connection = nullptr;
}

void P2PNetworking::update() {
    if (connection == nullptr && stateWrap.getState() == LinkState::CONNECTED) {
        peerDiscovery.stop();
        connection = std::make_unique<PeerConnection>(peerDiscovery.getSocketOwnership(), stateWrap);
    }
}

void P2PNetworking::sendBytes(std::vector<uint8_t> bytes) {
    update();
    return connection->sendBytes(bytes);
}

bool P2PNetworking::hasReceivedData() {
    update();
    return connection->receiver.hasReceivedData();
}

std::vector<uint8_t> P2PNetworking::popBytes() {
    update();
    return connection->receiver.popBytes();
}

LinkState P2PNetworking::getState() {
    return stateWrap.getState();
}

uint64_t P2PNetworking::getStartTimeInMilliseconds() {
    return startTimeInMilliseconds;
}