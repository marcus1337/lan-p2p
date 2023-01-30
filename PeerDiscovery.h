
#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H
#include <asio.hpp>

namespace peer2peer {
    enum class LinkStatus {
        DISCONNECTED, LOCATING, CONNECTED
    };

    class PeerDiscovery {

        LinkStatus state;

    public:

        PeerDiscovery();
        LinkStatus getState();

    };
}

#endif // !PEERDISCOVERY_H
