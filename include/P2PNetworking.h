
#ifndef P2PNETWORKING_H
#define P2PNETWORKING_H
#include "p2p/PeerDiscovery.h"
#include "p2p/PeerConnection.h"
#include "p2p/LinkState.h"
#include "p2p/IPDiscovery.h"
#include <memory>
#include <string>

namespace peer2peer {

    class P2PNetworking {

        uint64_t constructionTimeInMilliseconds = 0;
        uint64_t startTimeInMilliseconds = 0;
        LinkStateWrap stateWrap;
        PeerDiscovery peerDiscovery;
        std::unique_ptr<PeerConnection> connection;
        void update();

    public:
        P2PNetworking();
        ~P2PNetworking();

        void start();
        void stop();
        void sendBytes(std::vector<uint8_t> bytes);
        bool hasReceivedData();
        std::vector<uint8_t> popBytes();
        LinkState getState();
        uint32_t getStartTimeInMilliseconds();

    };

}

#endif // !P2PNETWORKING_H
