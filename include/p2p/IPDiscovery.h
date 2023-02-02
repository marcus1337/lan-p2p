
#ifndef IPDISCOVERY_H
#define IPDISCOVERY_H
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <asio.hpp>

namespace peer2peer {
    class IPDiscovery {

        std::atomic<bool> running;
        asio::io_context ioc;
        asio::ip::udp::socket detectSocket, sendSocket;
        std::thread detectBroadcastThread, broadcastThread;
        const int broadcastPort = 9678;
        std::mutex dataMutex;
        std::vector<std::string> ipAddresses;
        void addIP(std::string ip);
        std::vector<std::string> getIPs();
        std::vector<std::string> getLocalHostIPs();
        void detectBroadcasts();
        void broadcast();
        void removeLocalHostIPs(std::vector<std::string>& _ipAddresses);
        void initSockets();
        void initSendSocket();
        void initBroadcastSocket();
        void joinThreads();
    public:
        IPDiscovery();
        ~IPDiscovery();
        std::vector<std::string> getRemoteLANIPs();
    };
}


#endif