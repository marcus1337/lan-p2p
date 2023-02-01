
#ifndef PEERCONNECTION_H
#define PEERCONNECTION_H
#include "PeerReceiver.h"
#include "LinkState.h"
#include <mutex>

namespace peer2peer {
    class PeerConnection {
        asio::ip::tcp::socket& socket;
        std::mutex socketMutex;
        LinkStateWrap stateWrap;
    public:
        PeerConnection(asio::ip::tcp::socket& _socket);
        void sendMessage(std::string msg);
        PeerReceiver receiver;
        LinkState getState();
        void close();

    };
}

#endif // !PEERCONNECTION_H
