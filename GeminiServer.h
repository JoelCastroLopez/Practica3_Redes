#pragma once
#include "ProtocolServer.h"
#include <string>

class GeminiServer : public ProtocolServer {
public:
    GeminiServer(int port, const std::string& root);

protected:
    void handle_client(int client_fd) override;
};
