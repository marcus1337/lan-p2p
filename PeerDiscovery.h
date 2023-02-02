
#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H
#include <asio.hpp>
#include "IPDiscovery.h"
#include <mutex>
#include "LinkState.h"
#include <utility>
#include <atomic>

namespace peer2peer {

    class PeerDiscovery {

        IPDiscovery ipDiscovery;
        LinkStateWrap& stateWrap;
        std::mutex socketMutex;
        const int serverPort = 5431;
        const int waitTimeSeconds = 4;
        asio::io_context io_context;
        asio::ip::tcp::socket socket_;

        std::thread clientThread, serverThread;

        bool setSocket(asio::ip::tcp::socket&& _socket);
        void clientConnect(std::string ip);
        void clientSearch();
        void serverConnect();
        void serverSearch();
        void joinThreads();

    public:

        PeerDiscovery(LinkStateWrap& _stateWrap);
        ~PeerDiscovery();
        void start();
        void stop();
        asio::ip::tcp::socket&& getSocketOwnership();

    };
}

#endif // !PEERDISCOVERY_H
