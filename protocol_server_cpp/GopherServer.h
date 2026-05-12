#pragma once
#include "ProtocolServer.h"
#include <string>

class GopherServer : public ProtocolServer {
public:
    GopherServer(int port, const std::string& root,
                 const std::string& host = "localhost");

protected:
    void handle_client(int client_fd) override;

private:
    std::string host_;
};
