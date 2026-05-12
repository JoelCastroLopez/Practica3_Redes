# cmake_minimum_required(VERSION 3.13)
# project(protocol_server)

# find_package(Threads REQUIRED)
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x ")

# add_executable(protocol_server 
#     protocol_server.cpp 
#     ProtocolServer.cpp 
#     GopherServer.cpp 
#     GopherConnection.cpp
#     GeminiServer.cpp
#     GeminiConnection.cpp
# )

#target_link_libraries(protocol_server PRIVATE Threads::Threads)




cmake_minimum_required(VERSION 3.14)
project(protocol_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar OpenSSL en el sistema
find_package(OpenSSL REQUIRED)

add_executable(protocol_server
    protocol_server.cpp
    ProtocolServer.cpp
    GopherServer.cpp
    GopherConnection.cpp
    GeminiServer.cpp
    GeminiConnection.cpp
)

target_compile_options(protocol_server PRIVATE -Wall -Wextra)

# Enlazar con OpenSSL
target_link_libraries(protocol_server OpenSSL::SSL OpenSSL::Crypto)