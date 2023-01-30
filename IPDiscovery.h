
#ifndef IPDISCOVERY_H
#define IPDISCOVERY_H
#include <string>
#include <vector>

class IPDiscovery {

    static std::string getIPv4Str(int octet1, int octet2, int octet3, int octet4);
    bool canPing(std::string ip);
    std::string getLocalHostIP();
    std::vector<std::string> getIPAddressSearchRange();

    std::vector<std::string> getPingableRemoteLANIPs();
public:
    std::vector<std::string> getRemoteLANIPs();
};



#endif