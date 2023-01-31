#include "P2PNetworking.h"

using namespace peer2peer;

void P2PNetworking::closeConnection() {
    if (connection != nullptr) {
        connection->close();
    }
    connection = nullptr;
}

P2PNetworking::~P2PNetworking() {
    closeConnection();
}

void P2PNetworking::start() {
    closeConnection();
    peerDiscovery.startSearch();

}

void P2PNetworking::stop() {
    peerDiscovery.stopSearch();
    closeConnection();
}

void P2PNetworking::update() {
    if (connection == nullptr) {
        if (peerDiscovery.getState() == LinkState::CONNECTED) {
            connection = std::make_unique<PeerConnection>(peerDiscovery.getSocket());
        }
    }
}
void P2PNetworking::sendMessage(std::string msg) {
    return connection->sendMessage(msg);
}
bool P2PNetworking::hasMessage() {
    return connection->receiver.hasMessage();
}
std::string P2PNetworking::popMessage() {
    return connection->receiver.popMessage();
}

LinkState P2PNetworking::getState() {
    if (connection != nullptr) {
        return connection->getState();
    }
    else {
        return peerDiscovery.getState();
    }
}
