
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
        const int serverPort = 5431;
        const int waitTimeSeconds = 4;
        asio::io_context client_io_context, server_io_context;
        asio::ip::tcp::socket socket_;

        std::thread clientThread, serverThread;

        bool setSocket(asio::ip::tcp::socket&& _socket);
        void clientConnect(std::string ip, asio::ip::tcp::socket& _socket, bool& passedBlockingOperation);
        void clientSearch();
        void clientTryConnections();
        void serverAccept(asio::ip::tcp::socket& _socket, bool& passedBlockingOperation);
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
