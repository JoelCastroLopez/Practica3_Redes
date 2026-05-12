#include "GeminiServer.h"
#include "GeminiConnection.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstdio>

GeminiServer::GeminiServer(int port, const std::string& root, bool tls)
    : ProtocolServer(port, root), tls_(tls), ssl_ctx_(nullptr) {

    if (tls_) {
        SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
        if (!ctx) { ERR_print_errors_fp(stderr); exit(1); }

        if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
            fprintf(stderr, "ERROR: no se encuentra cert.pem\n");
            ERR_print_errors_fp(stderr); exit(1);
        }
        if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
            fprintf(stderr, "ERROR: no se encuentra key.pem\n");
            ERR_print_errors_fp(stderr); exit(1);
        }
        ssl_ctx_ = ctx;
    }
}

void GeminiServer::handle_client(int client_fd) {
    if (tls_) {
        SSL* ssl = SSL_new((SSL_CTX*)ssl_ctx_);
        SSL_set_fd(ssl, client_fd);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            return;
        }
        GeminiConnection conn(client_fd, root_, ssl);  // pasar ssl
        conn.handle();
        SSL_shutdown(ssl);
        SSL_free(ssl);
    } else {
        GeminiConnection conn(client_fd, root_, nullptr);
        conn.handle();
    }
}