cmake_minimum_required(VERSION 3.13)
project(protocol_server)

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x ")

add_executable(protocol_server 
    protocol_server.cpp 
    ProtocolServer.cpp 
    GopherServer.cpp 
    GopherConnection.cpp
    GeminiServer.cpp
    GeminiConnection.cpp
)

target_link_libraries(protocol_server PRIVATE Threads::Threads)
