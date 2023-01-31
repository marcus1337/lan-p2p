
#include "P2PNetworking.h"
#include <iostream>

using namespace peer2peer;

int main()
{
    P2PNetworking p2pNetworking;
    p2pNetworking.start();

    while (p2pNetworking.getState() != LinkState::DISCONNECTED) {
        std::cout << "state " << (int) p2pNetworking.getState() << "\n";
        p2pNetworking.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Closing program.\n";

    return 0;
}