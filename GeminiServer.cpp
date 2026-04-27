#include "GeminiServer.h"
#include "GeminiConnection.h"

GeminiServer::GeminiServer(int port, const std::string& root)
    : ProtocolServer(port, root) {}

void GeminiServer::handle_client(int client_fd) {
    GeminiConnection conn(client_fd, root_);
    conn.handle();
}
