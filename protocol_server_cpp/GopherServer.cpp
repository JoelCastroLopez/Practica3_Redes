#include "GopherServer.h"
#include "GopherConnection.h"

GopherServer::GopherServer(int port, const std::string& root,
                           const std::string& host)
    : ProtocolServer(port, root), host_(host) {}

void GopherServer::handle_client(int client_fd) {
    GopherConnection conn(client_fd, root_, host_, port_);
    conn.handle();
}
