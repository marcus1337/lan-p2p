
#ifndef LINKSTATE_H
#define LINKSTATE_H
#include <mutex>
#include <asio.hpp>

namespace peer2peer {

    enum class LinkState {
        DISCONNECTED = 0, LOCATING, CONNECTED
    };

    class LinkStateWrap {
        LinkState state;
        std::mutex stateMutex;

    public:

        LinkStateWrap();
        LinkState getState();
        void setState(LinkState _state);
        bool trySetStateConnected();
        void setDisconnectedIfLocating();
        static LinkState getSocketState(asio::ip::tcp::socket& _socket);

    };

}
#endif
