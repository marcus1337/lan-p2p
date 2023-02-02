cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
project(test_project)

add_subdirectory("${PROJECT_SOURCE_DIR}/.." "${CMAKE_CURRENT_BINARY_DIR}/p2plib")
add_executable(${PROJECT_NAME} "test.cpp")
target_link_libraries(${PROJECT_NAME} p2plib)

