
#include "IPDiscovery.h"
#include "PeerDiscovery.h"
#include <iostream>

#include <thread>
#include <chrono>

using namespace peer2peer;

int main()
{
    /*IPDiscovery ipDiscovery;
    auto remoteIPs = ipDiscovery.getRemoteLANIPs();
    for (auto ip : remoteIPs) {
        std::cout << "[" << ip << "]\n";
    }*/

    PeerDiscovery peerDiscovery;
    peerDiscovery.startSearch();

    while (peerDiscovery.getState() == LinkState::LOCATING) {

        std::cout << "locating...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    }

    std::cout << "Found something...........\n";

    return 0;
}