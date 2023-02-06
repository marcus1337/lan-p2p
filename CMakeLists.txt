cmake_minimum_required(VERSION 3.10)
project(p2plib)
set(CMAKE_CXX_STANDARD 20)

find_package(asio REQUIRED)

add_library(${PROJECT_NAME} STATIC 
"src/P2PNetworking.cpp" 
"src/IPDiscovery.cpp"
"src/LinkStateWrap.cpp"
"src/PeerConnection.cpp"
"src/PeerDiscovery.cpp"
"src/PeerReceiver.cpp"
"src/Handshake.cpp"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
     ${asio_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${asio_LIBRARIES})

