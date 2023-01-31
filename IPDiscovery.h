
#ifndef IPDISCOVERY_H
#define IPDISCOVERY_H
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>

namespace peer2peer {
    class IPDiscovery {

        std::atomic<bool> running;
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
    public:
        IPDiscovery();
        ~IPDiscovery();
        std::vector<std::string> getRemoteLANIPs();
    };
}


#endif