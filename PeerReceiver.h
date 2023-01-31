
#ifndef PEERRECEIVER_H
#define PEERRECEIVER_H
#include <vector>
#include <asio.hpp>
#include <mutex>
#include <string>
#include <thread>

namespace peer2peer {
    class PeerReceiver {

        std::mutex msgMutex;
        asio::ip::tcp::socket& socket;
        std::vector<std::string> messages;
        std::thread receiveThread;

        void addMessage(std::string msg);
        void receive();
    public:
        PeerReceiver(asio::ip::tcp::socket& _socket);
        ~PeerReceiver();
        std::vector<std::string> popMessages();
        bool hasMessage();

    };
}

#endif // !PEERRECEIVER_H
