#include "IPDiscovery.h"
#include <thread>
#include <future>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream>
#include <memory>
#include <asio.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>

using namespace peer2peer;


#ifdef _WIN32
#pragma comment(lib, "iphlpapi.lib")
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <WS2tcpip.h>
#include <winsock2.h>

std::vector<std::string> IPDiscovery::getLocalHostIPs() {

    std::vector<std::string> ips;

    ULONG ulOutBufLen = 0;
    DWORD dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &ulOutBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
        auto pAddresses = std::make_unique<IP_ADAPTER_ADDRESSES[]>(ulOutBufLen);
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses.get(), &ulOutBufLen);
        if (dwRetVal == NO_ERROR)
        {
            for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses.get(); pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next)
            {
                for (IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != nullptr; pUnicast = pUnicast->Next)
                {
                    auto pAddress = reinterpret_cast<SOCKADDR_STORAGE*>(pUnicast->Address.lpSockaddr);
                    if (pAddress->ss_family == AF_INET)
                    {
                        char ip_address[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(reinterpret_cast<sockaddr_in*>(pAddress)->sin_addr), ip_address, INET_ADDRSTRLEN);
                        ips.push_back(ip_address);
                    }
                }
            }
        }
    }

    return ips;
}

#endif


void IPDiscovery::broadcast() {
    asio::io_context ioc;
    asio::ip::udp::socket socket(ioc);
    socket.open(asio::ip::udp::v4());
    socket.set_option(asio::socket_base::broadcast(true));
    socket.set_option(asio::socket_base::reuse_address(true));
    asio::ip::udp::endpoint broadcast_endpoint(
        asio::ip::address_v4::broadcast(), broadcastPort
    );

    while (running) {
        std::array<char, 1> send_buffer = { {0} };
        socket.send_to(asio::buffer(send_buffer), broadcast_endpoint);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

std::vector<std::string> IPDiscovery::getRemoteLANIPs() {
    auto _ipAddresses = getIPs();
    removeLocalHostIPs(_ipAddresses);
    return _ipAddresses;
}

void IPDiscovery::removeLocalHostIPs(std::vector<std::string>& _ipAddresses) {
    std::vector<std::string> localHostIPs = getLocalHostIPs();
    for (const auto& localHostIP : localHostIPs) {
        _ipAddresses.erase(std::remove(_ipAddresses.begin(), _ipAddresses.end(), localHostIP), _ipAddresses.end());
    }
}

void IPDiscovery::addIP(std::string ip) {
    std::lock_guard<std::mutex> lock(dataMutex);
    auto it = std::find(ipAddresses.begin(), ipAddresses.end(), ip);
    if (it == ipAddresses.end()) {
        std::cout << "Detected IP: " << ip << "\n";
        ipAddresses.push_back(ip);
    }
}

std::vector<std::string> IPDiscovery::getIPs() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return ipAddresses;
}


void IPDiscovery::detectBroadcasts() {
    asio::io_context ioc;
    asio::ip::udp::socket socket(ioc);
    socket.open(asio::ip::udp::v4());
    socket.set_option(asio::socket_base::reuse_address(true));
    socket.set_option(asio::socket_base::broadcast(true));
    asio::ip::udp::endpoint listen_endpoint(
        asio::ip::make_address("0.0.0.0"), broadcastPort
    );
    socket.bind(listen_endpoint);
    std::array<char, 1024> recv_buffer;
    while (running) {
        try {
            asio::ip::udp::endpoint sender_endpoint;
            auto bytes_received = socket.receive_from(
                asio::buffer(recv_buffer), sender_endpoint
            );
            if (bytes_received > 0) {
                addIP(sender_endpoint.address().to_string());
            }
        }
        catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

IPDiscovery::IPDiscovery() {
    running = true;
    detectBroadcastThread = std::thread(&IPDiscovery::detectBroadcasts, this);
    broadcastThread = std::thread(&IPDiscovery::broadcast, this);
}

IPDiscovery::~IPDiscovery() {
    running = false;
    if (detectBroadcastThread.joinable()) {
        detectBroadcastThread.join();
    }
    if (broadcastThread.joinable()) {
        broadcastThread.join();
    }
}