
#ifndef P2PNETWORKING_H
#define P2PNETWORKING_H
#include "PeerDiscovery.h"
#include "PeerConnection.h"
#include "LinkState.h"
#include "IPDiscovery.h"
#include <memory>
#include <string>

namespace peer2peer {

    class P2PNetworking {

        PeerDiscovery peerDiscovery;
        std::unique_ptr<PeerConnection> connection;
        void closeConnection();

    public:

        ~P2PNetworking();

        void start();
        void stop();
        void update();
        void sendMessage(std::string msg);
        bool hasMessage();
        std::string popMessage();
        LinkState getState();

    };

}

#endif // !P2PNETWORKING_H
