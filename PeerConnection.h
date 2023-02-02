
#ifndef PEERCONNECTION_H
#define PEERCONNECTION_H
#include "PeerReceiver.h"
#include "LinkState.h"
#include <mutex>

namespace peer2peer {
    class PeerConnection {
        LinkStateWrap& stateWrap;
        asio::ip::tcp::socket& socket;
    public:
        PeerConnection(asio::ip::tcp::socket&& _socket, LinkStateWrap& _stateWrap);
        ~PeerConnection();
        void sendMessage(std::string msg);
        PeerReceiver receiver;
    };
}

#endif // !PEERCONNECTION_H
