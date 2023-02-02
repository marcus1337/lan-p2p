
#ifndef PEERRECEIVER_H
#define PEERRECEIVER_H
#include <asio.hpp>
#include <mutex>
#include <string>
#include <thread>
#include <queue>
#include "LinkState.h"

namespace peer2peer {
    class PeerReceiver {

        LinkStateWrap& stateWrap;
        std::mutex msgMutex;
        asio::ip::tcp::socket& socket;
        std::queue<std::string> messages;
        std::thread receiveThread;
        void addMessage(std::string msg);
        void receive();
    public:
        PeerReceiver(asio::ip::tcp::socket& _socket, LinkStateWrap& _stateWrap);
        ~PeerReceiver();
        std::string popMessage();
        bool hasMessage();

    };
}

#endif // !PEERRECEIVER_H
