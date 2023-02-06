
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

        static constexpr std::size_t MAX_STORED_MESSAGES = 500;
        static constexpr std::size_t MAX_MESSAGE_SIZE = 30000;
        LinkStateWrap& stateWrap;
        std::mutex msgMutex;
        asio::ip::tcp::socket& socket;
        std::queue<std::vector<uint8_t>> messages;
        std::thread receiveThread;
        void addBytes(std::vector<uint8_t> msg);
        void receive();
    public:
        PeerReceiver(asio::ip::tcp::socket& _socket, LinkStateWrap& _stateWrap);
        ~PeerReceiver();
        std::vector<uint8_t> popBytes();
        bool hasReceivedData();

    };
}

#endif // !PEERRECEIVER_H
