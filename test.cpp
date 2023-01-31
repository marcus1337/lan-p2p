
#include "P2PNetworking.h"
#include <chrono>
#include <format>
#include <iostream>
#include <string>

using namespace peer2peer;


std::string getTime() {
    const auto now = std::chrono::system_clock::now();
    auto str = std::format("{:%d-%m-%Y %H:%M:%OS}", now);
    return str;
}

int main()
{
    P2PNetworking p2pNetworking;
    p2pNetworking.start();

    std::string timeStr = getTime();

    while (p2pNetworking.getState() != LinkState::DISCONNECTED) {
        std::cout << "state " << (int) p2pNetworking.getState() << "\n";
        p2pNetworking.update();
        if (p2pNetworking.getState() == LinkState::CONNECTED) {
            p2pNetworking.sendMessage(timeStr);
            if (p2pNetworking.hasMessage()) {
                std::cout << "Msg [" << p2pNetworking.popMessage() << "\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Closing program.\n";

    return 0;
}