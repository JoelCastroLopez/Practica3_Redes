#pragma once
#include "ProtocolServer.h"
#include <string>

class GeminiServer : public ProtocolServer {
    public:
        GeminiServer(int port, const std::string& root, bool tls = false);

    protected:
        void handle_client(int client_fd) override;

    private:
        bool tls_;
        void* ssl_ctx_;
};
