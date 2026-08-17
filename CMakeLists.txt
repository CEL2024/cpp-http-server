cmake_minimum_required(VERSION 3.16)

project(cpp_http_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/main.cpp
    src/socket.cpp
    src/HTTPparser.cpp
    src/ClientConnections.cpp
)