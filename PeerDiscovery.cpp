#include "PeerDiscovery.h"
#include <iostream>
#include <exception>
#include <future>

using namespace peer2peer;
using namespace asio::ip;


PeerDiscovery::PeerDiscovery() : stateWrap(LinkState::DISCONNECTED), socket_(io_context) {

}

PeerDiscovery::~PeerDiscovery() {
    stopSearch();
}

bool PeerDiscovery::setSocket(asio::ip::tcp::socket _socket) {
    std::lock_guard<std::mutex> lock(socketMutex);
    if (LinkStateWrap::getSocketState(_socket) == LinkState::CONNECTED && stateWrap.trySetStateConnected()) {
        socket_ = std::move(_socket);
        return true;
    }
    return false;
}

void PeerDiscovery::clientConnect(std::string ip) {
    tcp::socket _socket(io_context);
    asio::ip::address ipAddress = asio::ip::make_address(ip);
    tcp::endpoint endpoint(ipAddress, serverPort);

    auto future = std::async([&_socket, endpoint]() {
        _socket.connect(endpoint);
        });
    auto status = future.wait_for(std::chrono::seconds(waitTimeSeconds));
    if (status == std::future_status::ready && setSocket(std::move(_socket))) {
        std::cout << "Connected [" << ip << "]\n";
    }
    else {
        std::cout << "Client [" << ip << "] timeout.\n";
    }
}

void PeerDiscovery::clientSearch() {
    std::cout << "clientSearch()\n";
    IPDiscovery ipDiscovery;
    std::vector<std::future<void>> clients;

    while (getState() == LinkState::LOCATING) {
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
    if (status == std::future_status::ready && setSocket(std::move(_socket))) {
        std::cout << "Server connected\n";
    }
}

void PeerDiscovery::serverSearch() {
    std::cout << "serverSearch()\n";
    while (getState() == LinkState::LOCATING) {
        try {
            serverConnect();
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

LinkState PeerDiscovery::getState() {
    return stateWrap.getState();
}

asio::ip::tcp::socket&& PeerDiscovery::getSocket() {
    return std::move(socket_);
}

void PeerDiscovery::startSearch() {
    stateWrap.setState(LinkState::LOCATING);
    clientThread = std::thread(&PeerDiscovery::clientSearch, this);
    serverThread = std::thread(&PeerDiscovery::serverSearch, this);
}

void PeerDiscovery::stopSearch() {
    stateWrap.stopLocating();
    joinThreads();
}

void PeerDiscovery::joinThreads() {
    if (clientThread.joinable())
        clientThread.join();
    if (serverThread.joinable())
        serverThread.join();
}