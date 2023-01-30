
#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H
#include <asio.hpp>
#include "IPDiscovery.h"
#include <mutex>

namespace peer2peer {
    enum class LinkState {
        DISCONNECTED, LOCATING, CONNECTED
    };

    class PeerDiscovery {

        std::mutex stateMutex, socketMutex;
        const int serverPort = 5431;
        const int waitTimeSeconds = 5;

        LinkState state;
        asio::io_context io_context;
        asio::ip::tcp::socket socket_;

        std::thread clientThread, serverThread;

        void setSocket(asio::ip::tcp::socket _socket);
        void clientConnect(std::string ip);
        void clientSearch();
        void serverConnect();
        void serverSearch();
        void setState(LinkState _state);
        void joinThreads();
        bool trySetStateConnected();

    public:

        PeerDiscovery();
        ~PeerDiscovery();

        void startSearch();
        void stopSearch();

        LinkState getState();
        asio::ip::tcp::socket& socket();

    };
}

#endif // !PEERDISCOVERY_H
