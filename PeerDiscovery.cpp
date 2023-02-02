#include "PeerDiscovery.h"
#include <iostream>
#include <exception>
#include <future>

using namespace peer2peer;
using namespace asio::ip;


PeerDiscovery::PeerDiscovery(LinkStateWrap& _stateWrap) : stateWrap(_stateWrap), socket_(io_context) {

}

PeerDiscovery::~PeerDiscovery() {
    stop();
}

bool PeerDiscovery::setSocket(asio::ip::tcp::socket&& _socket) {
    std::lock_guard<std::mutex> lock(socketMutex);
    if (LinkStateWrap::getSocketState(_socket) == LinkState::CONNECTED && stateWrap.trySetStateConnected()) {
        socket_ = std::move(_socket);
        return true;
    }
    return false;
}

void PeerDiscovery::clientConnect(std::string ip) {

    std::cout << "clientConnect()\n";

     tcp::socket _socket(io_context);
     asio::ip::address ipAddress = asio::ip::make_address(ip);
     tcp::endpoint endpoint(ipAddress, serverPort);

     auto future = std::async([&_socket, endpoint]() {
         _socket.connect(endpoint);
         });
     auto status = future.wait_for(std::chrono::seconds(waitTimeSeconds));
     if (status != std::future_status::ready) {
         _socket.close();
         std::cout << "Client [" << ip << "] timeout.\n";
     }else {
         setSocket(std::move(_socket));
     }
}

void PeerDiscovery::clientSearch() {
    std::cout << "clientSearch()\n";
    std::vector<std::future<void>> clients;

    while (stateWrap.getState() == LinkState::LOCATING) {
        clients.clear();
        for (std::string ip : ipDiscovery.getRemoteLANIPs()) {
            clients.push_back(std::async(&PeerDiscovery::clientConnect, this, ip));
        }
        for (auto& client : clients) {
            client.wait();
        }
    }

}

void PeerDiscovery::serverConnect() {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), serverPort));
    tcp::socket _socket(io_context);
    auto future = std::async([&_socket, &acceptor]() {
        acceptor.accept(_socket);
        });
    auto status = future.wait_for(std::chrono::seconds(waitTimeSeconds));
    if (status == std::future_status::ready) {
        setSocket(std::move(_socket));
        std::cout << "Server connected\n";
    }
}

void PeerDiscovery::serverSearch() {
    std::cout << "serverSearch()\n";
    while (stateWrap.getState() == LinkState::LOCATING) {
        try {
            serverConnect();
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

asio::ip::tcp::socket&& PeerDiscovery::getSocketOwnership() {
    return std::move(socket_);
}

void PeerDiscovery::start() {
    stateWrap.setState(LinkState::LOCATING);
    //clientThread = std::thread(&PeerDiscovery::clientSearch, this);
    serverThread = std::thread(&PeerDiscovery::serverSearch, this);
}

void PeerDiscovery::stop() {
    if (stateWrap.getState() == LinkState::LOCATING)
        stateWrap.setState(LinkState::DISCONNECTED);
    joinThreads();
}

void PeerDiscovery::joinThreads() {
    std::cout << "PeerDiscovery clientThread\n";
    if (clientThread.joinable())
        clientThread.join();
    std::cout << "PeerDiscovery serverThread\n";
    if (serverThread.joinable())
        serverThread.join();
    std::cout << "PeerDiscovery threads joined\n";
}