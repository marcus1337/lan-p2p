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
    if (LinkStateWrap::getSocketState(_socket) == LinkState::CONNECTED && stateWrap.trySetStateConnected()) {
        socket_ = std::move(_socket);
        return true;
    }
    else {
        _socket.close();
        return false;
    }
}

void PeerDiscovery::clientConnect(std::string ip, asio::ip::tcp::socket& _socket, bool& passedBlockingOperation) {
    std::cout << "clientConnect()\n";
    asio::ip::address ipAddress = asio::ip::make_address(ip);
    tcp::endpoint endpoint(ipAddress, serverPort);
    asio::error_code ec;
    _socket.connect(endpoint, ec);
    passedBlockingOperation = true;
    if (ec) {
        std::cout << "clientConnect() error: " << ec.message() << "\n";
        _socket.close();
    }
    else {
        setSocket(std::move(_socket));
    }
}

void PeerDiscovery::clientTryConnections() {
    std::vector<std::future<void>> clients;
    std::vector<std::shared_ptr<bool>> passedBlockingCallPtrs;
    std::vector<std::shared_ptr<tcp::socket>> socketPtrs;

    for (auto ip : ipDiscovery.getRemoteLANIPs()) {
        std::shared_ptr<tcp::socket> socketPtr = std::make_shared<tcp::socket>(io_context);
        std::shared_ptr<bool> passedBlock = std::make_shared<bool>(false);
        clients.push_back(std::async(&PeerDiscovery::clientConnect, this, ip, std::ref(*socketPtr), std::ref(*passedBlock)));
        socketPtrs.push_back(socketPtr);
        passedBlockingCallPtrs.push_back(passedBlock);
    }

    std::chrono::system_clock::time_point clientTime = std::chrono::system_clock::now() + std::chrono::seconds(waitTimeSeconds);
    for (int i = 0; i < clients.size(); i++) {
        clients[i].wait_until(clientTime);
        if (*passedBlockingCallPtrs[i] == false) {
            socketPtrs[i]->close();
        }
        clients[i].wait();
    }
}

void PeerDiscovery::clientSearch() {
    std::cout << "clientSearch()\n";
    while (stateWrap.getState() == LinkState::LOCATING) {
        clientTryConnections();
    }
}

void PeerDiscovery::serverAccept(asio::ip::tcp::socket& _socket, bool& passedBlockingOperation) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), serverPort));
    asio::error_code ec;
    acceptor.accept(_socket, ec);
    passedBlockingOperation = true;
    if (ec) {
        std::cout << "serverConnect() error " << ec.message() << "\n";
        _socket.close();
    }
    else {
        setSocket(std::move(_socket));
    }
}

void PeerDiscovery::serverSearch() {
    std::cout << "serverSearch()\n";
    while (stateWrap.getState() == LinkState::LOCATING) {
        tcp::socket _socket(io_context);
        bool passedBlockingOperation = false;
        auto future = std::async(&PeerDiscovery::serverAccept, this, std::ref(_socket), std::ref(passedBlockingOperation));
        future.wait_for(std::chrono::seconds(waitTimeSeconds));
        if (!passedBlockingOperation) {
            _socket.close();
        }
        future.wait();
    }
}

asio::ip::tcp::socket&& PeerDiscovery::getSocketOwnership() {
    return std::move(socket_);
}

void PeerDiscovery::start() {
    stateWrap.setState(LinkState::LOCATING);
    clientThread = std::thread(&PeerDiscovery::clientSearch, this);
    serverThread = std::thread(&PeerDiscovery::serverSearch, this);
}

void PeerDiscovery::stop() {
    stateWrap.setDisconnectedIfLocating();
    joinThreads();
}

void PeerDiscovery::joinThreads() {
    std::cout << "join clientThread\n";
    if (clientThread.joinable())
        clientThread.join();
    std::cout << "join serverThread\n";
    if (serverThread.joinable())
        serverThread.join();
    std::cout << "PeerDiscovery threads joined\n";
}

