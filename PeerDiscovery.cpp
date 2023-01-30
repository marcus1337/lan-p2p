#include "PeerDiscovery.h"

using namespace peer2peer;


PeerDiscovery::PeerDiscovery() : state(LinkStatus::DISCONNECTED) {

}

LinkStatus PeerDiscovery::getState() {
    return state;
}