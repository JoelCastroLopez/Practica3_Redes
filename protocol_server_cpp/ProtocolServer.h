#pragma once
#include <string>

// ─── Clase base para servidores TCP ──────────────────────────────────────────
class ProtocolServer {
public:
    ProtocolServer(int port, const std::string& root);
    virtual ~ProtocolServer() = default;

    void run();  // bucle principal

protected:
    // Cada subclase implementa cómo atender a un cliente concreto
    virtual void handle_client(int client_fd) = 0;

    int port_;
    std::string root_;  // directorio raíz que sirve el servidor

private:
    // NIVEL 1: crear socket TCP (socket + bind + listen)
    int define_socket_TCP();

    int listen_fd_ = -1;
};
