
#include "IPDiscovery.h"
#include <iostream>

int main()
{
    IPDiscovery ipDiscovery;
    auto remoteIPs = ipDiscovery.getRemoteLANIPs();
    for (auto ip : remoteIPs) {
        std::cout << "[" << ip << "]\n";
    }

    return 0;
}