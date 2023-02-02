
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

    while (true) {

        p2pNetworking.start();

        while (p2pNetworking.getState() == LinkState::LOCATING) {
            std::cout << "Locating...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        while (p2pNetworking.getState() == LinkState::CONNECTED) {
            std::cout << "Connected...\n";
            p2pNetworking.sendMessage(getTime());
            if (p2pNetworking.hasMessage()) {
                std::cout << "Msg [" << p2pNetworking.popMessage() << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << "before stop\n";
        p2pNetworking.stop();
        std::cout << "after stop\n";
    }

    std::cout << "Closing program.\n";

    return 0;
}