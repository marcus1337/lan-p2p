#include "IPDiscovery.h"
#include <thread>
#include <future>
#include <iostream>
#include <vector>
#include <asio.hpp>
#include <chrono>
#include <thread>
#include <sstream>


#ifdef _WIN32
#pragma comment(lib, "iphlpapi.lib")
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <WS2tcpip.h>

bool IPDiscovery::canPing(std::string ip) {
    IPAddr dstAddress = 0;
    int result = inet_pton(AF_INET, ip.c_str(), &dstAddress);
    if (result != 1)
        return false;

    unsigned char sendData[] = "ping";
    const DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
    PICMP_ECHO_REPLY pReply[replySize];

    DWORD replyTimeout = 1000; // timeout in milliseconds
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
        return false;
    DWORD replyStatus = IcmpSendEcho(hIcmpFile, dstAddress, sendData, sizeof(sendData), nullptr, pReply, replySize, replyTimeout);
    IcmpCloseHandle(hIcmpFile);
    return replyStatus != 0;
}

#endif

std::string IPDiscovery::getIPv4Str(int octet1, int octet2, int octet3, int octet4) {
    std::stringstream ipStrStream;
    ipStrStream << octet1 << "." << octet2 << "." << octet3 << "." << octet4;
    return ipStrStream.str();
}

std::string IPDiscovery::getLocalHostIP() {
    return "";
}

std::vector<std::string> IPDiscovery::getIPAddressSearchRange() {
    std::vector<std::string> ips;
    for (int i = 2; i <= 255; i++) {
        ips.push_back(getIPv4Str(192, 168, 1, i));
    }
    return ips;
}

std::vector<std::string> IPDiscovery::getPingableRemoteLANIPs() {
    std::vector<std::future<bool>> futures;
    auto ips = getIPAddressSearchRange();
    for (auto ip : ips) {
        futures.push_back(std::async(std::launch::async, &IPDiscovery::canPing, this, ip));
    }
    std::vector<std::string> pingableIPs;
    for (int i = 0; i < ips.size(); i++) {
        futures[i].wait();
        if (futures[i].get())
            pingableIPs.push_back(ips[i]);
    }
    return pingableIPs;
}

std::vector<std::string> IPDiscovery::getRemoteLANIPs() {
    auto pingableIPs = getPingableRemoteLANIPs();

    std::string localHostIP = getLocalHostIP();
    pingableIPs.erase(std::remove_if(
        pingableIPs.begin(), pingableIPs.end(),
        [localHostIP](const std::string& ip) {
            return localHostIP == ip;
        }), pingableIPs.end());

    return pingableIPs;
}