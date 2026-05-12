#include "ProtocolServer.h"

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

ProtocolServer::ProtocolServer(int port, const std::string& root)
    : port_(port), root_(root) {}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 1 — define_socket_TCP
//   1. socket()  → crea el socket TCP
//   2. bind()    → asocia IP + puerto
//   3. listen()  → pone el socket en modo escucha
// ════════════════════════════════════════════════════════════════════════
int ProtocolServer::define_socket_TCP() {
    struct sockaddr_in sin;
    int s;

    // Crear socket TCP
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        throw std::runtime_error(std::string("socket(): ") + strerror(errno));

    // Permitir reusar el puerto inmediatamente
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Rellenar estructura de dirección y hacer bind
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;   // acepta conexiones de cualquier IP
    sin.sin_port        = htons(port_); // puerto en orden de red 

    if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
        throw std::runtime_error(std::string("bind(): ") + strerror(errno));

    // Poner en escucha
    if (listen(s, 10) < 0)
        throw std::runtime_error(std::string("listen(): ") + strerror(errno));

    return s;
}

// Bucle principal
void ProtocolServer::run() {
    listen_fd_ = define_socket_TCP();
    printf("Servidor escuchando en puerto %d (raiz: %s)\n", port_, root_.c_str());

    struct sockaddr_in client_addr;
    socklen_t alen = sizeof(client_addr);

    while (true) {
        // accept() bloquea hasta que llega una conexión y devuelve el nuevo fd
        int client_fd = accept(listen_fd_,
                               (struct sockaddr*)&client_addr, &alen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Crear proceso hijo para atender al cliente
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
        } else if (pid == 0) {
            // Proceso hijo
            close(listen_fd_);      // el hijo no necesita el socket padre
            handle_client(client_fd);
            close(client_fd);
            _exit(0);
        } else {
            // Proceso padre
            close(client_fd);
            waitpid(-1, nullptr, WNOHANG); // recoger zombies sin bloquear
        }
    }
}
