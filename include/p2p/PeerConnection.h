
#ifndef PEERCONNECTION_H
#define PEERCONNECTION_H
#include "PeerReceiver.h"
#include "LinkState.h"
#include <mutex>
#include <vector>

namespace peer2peer {
    class PeerConnection {
        LinkStateWrap& stateWrap;
        asio::ip::tcp::socket& socket;
    public:
        PeerConnection(asio::ip::tcp::socket&& _socket, LinkStateWrap& _stateWrap);
        ~PeerConnection();
        void sendBytes(std::vector<uint8_t> bytes);
        void stop();
        PeerReceiver receiver;
    };
}

#endif // !PEERCONNECTION_H
