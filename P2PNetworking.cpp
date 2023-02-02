#include "P2PNetworking.h"
#include <iostream>

using namespace peer2peer;

P2PNetworking::P2PNetworking() : peerDiscovery(stateWrap) {

}

P2PNetworking::~P2PNetworking() {
    stop();
}

void P2PNetworking::start() {
    stop();
    peerDiscovery.start();
}

void P2PNetworking::stop() {
    peerDiscovery.stop();
    connection = nullptr;
}

void P2PNetworking::update() {
    if (connection == nullptr && stateWrap.getState() == LinkState::CONNECTED) {
        peerDiscovery.stop();
        connection = std::make_unique<PeerConnection>(peerDiscovery.getSocketOwnership(), stateWrap);
    }
}

void P2PNetworking::sendMessage(std::string msg) {
    update();
    return connection->sendMessage(msg);
}
bool P2PNetworking::hasMessage() {
    update();
    return connection->receiver.hasMessage();
}
std::string P2PNetworking::popMessage() {
    update();
    return connection->receiver.popMessage();
}

LinkState P2PNetworking::getState() {
    return stateWrap.getState();
}

