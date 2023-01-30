
#include "IPDiscovery.h"
#include "PeerDiscovery.h"
#include <iostream>

using namespace peer2peer;

int main()
{
    IPDiscovery ipDiscovery;
    auto remoteIPs = ipDiscovery.getRemoteLANIPs();
    for (auto ip : remoteIPs) {
        std::cout << "[" << ip << "]\n";
    }

    return 0;
}