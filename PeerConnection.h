
#ifndef PEERCONNECTION_H
#define PEERCONNECTION_H
#include "PeerReceiver.h"
#include "LinkState.h"

namespace peer2peer {
    class PeerConnection {
        asio::ip::tcp::socket socket;

    public:
        PeerConnection(asio::ip::tcp::socket _socket);
        void sendMessage(std::string msg);
        PeerReceiver receiver;
        LinkState getState();
        void close();

    };
}

#endif // !PEERCONNECTION_H
