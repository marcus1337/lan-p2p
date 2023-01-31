#include "PeerDiscovery.h"
#include <iostream>
#include <exception>
#include <future>

using namespace peer2peer;
using namespace asio::ip;


PeerDiscovery::PeerDiscovery() : state(LinkState::DISCONNECTED), socket_(io_context) {

}

PeerDiscovery::~PeerDiscovery(){
    stopSearch();
}

void PeerDiscovery::setSocket(asio::ip::tcp::socket _socket) {
    std::lock_guard<std::mutex> lock(socketMutex);
    if (trySetStateConnected()) {
        socket_ = std::move(_socket);
    }
}

void PeerDiscovery::clientConnect(std::string ip) {
    tcp::socket _socket(io_context);

    asio::steady_timer timer(io_context, std::chrono::seconds(waitTimeSeconds));
    timer.async_wait([&_socket](const std::error_code& error) {
        if (!error)
        {
            _socket.close();
        }
        });

    asio::ip::address ipAddress = asio::ip::make_address(ip);
    tcp::endpoint endpoint(ipAddress, serverPort);
    std::error_code ec;
    _socket.connect(endpoint, ec);
    timer.cancel();

    try {
        if (_socket.is_open() && _socket.remote_endpoint().address() != asio::ip::address()) {
            std::cout << "client " << ip << "\n";
            setSocket(std::move(_socket));
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << "\n";
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
    asio::steady_timer timer(io_context, std::chrono::seconds(waitTimeSeconds));
    timer.async_wait([&_socket](const std::error_code& error) {
        if (!error)
        {
            _socket.close();
        }
        });
    acceptor.accept(_socket);

    if (_socket.is_open()) {
        setSocket(std::move(_socket));
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

void PeerDiscovery::setState(LinkState _state) {
    std::lock_guard<std::mutex> lock(stateMutex);
    state = _state;
}

bool PeerDiscovery::trySetStateConnected() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (state == LinkState::LOCATING) {
        state = LinkState::CONNECTED;
        return true;
    }
    return false;
}

LinkState PeerDiscovery::getState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return state;
}

asio::ip::tcp::socket& PeerDiscovery::socket() {
    return socket_;
}

void PeerDiscovery::startSearch() {
    joinThreads();
    socket_.close();
    setState(LinkState::LOCATING);
    clientThread = std::thread(&PeerDiscovery::clientSearch, this);
    serverThread = std::thread(&PeerDiscovery::serverSearch, this);
}

void PeerDiscovery::stopSearch() {
    if (socket_.is_open()) {
        setState(LinkState::CONNECTED);
    }
    else {
        setState(LinkState::DISCONNECTED);
    }
    joinThreads();
}

void PeerDiscovery::joinThreads() {
    if (clientThread.joinable())
        clientThread.join();
    if (serverThread.joinable())
        serverThread.join();
}