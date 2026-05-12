# TLS Setup Guide for Gemini Protocol

## Overview

The Gemini protocol specification requires TLS encryption. This implementation supports **optional TLS**, allowing students to:
1. First implement the basic Gemini protocol without TLS
2. Then add TLS as an advanced feature for bonus points

## Generating Self-Signed Certificates

For development and testing, you can generate self-signed certificates:

```bash
cd protocol_server_cpp

# Generate certificate and private key (valid for 365 days)
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes

# You'll be prompted for certificate information
# For testing, you can use default values or:
# Country: ES
# State: Tenerife
# City: San Cristóbal de La Laguna
# Organization: Universidad de La Laguna
# Common Name: localhost
```

This creates two files:
- `cert.pem` - The certificate file
- `key.pem` - The private key file

## Implementing TLS in Your Server

### Step 1: Add OpenSSL to CMakeLists.txt

```cmake
find_package(OpenSSL REQUIRED)
target_link_libraries(protocol_server PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

### Step 2: Initialize SSL Context (GeminiServer.cpp)

```cpp
#include <openssl/ssl.h>
#include <openssl/err.h>

void GeminiServer::init_ssl() {
    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    // Create SSL context
    ssl_ctx = SSL_CTX_new(TLS_server_method());
    if (!ssl_ctx) {
        errexit("Unable to create SSL context\n");
    }
    
    // Load certificate
    if (SSL_CTX_use_certificate_file(ssl_ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        errexit("Error loading certificate\n");
    }
    
    // Load private key
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        errexit("Error loading private key\n");
    }
}
```

### Step 3: Wrap Socket with SSL (GeminiConnection.cpp)

```cpp
bool GeminiConnection::init_tls() {
    if (!ssl_ctx) {
        return false;
    }
    
    // Create SSL structure
    ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        return false;
    }
    
    // Attach socket to SSL
    SSL_set_fd(ssl, socket_fd);
    
    // Perform TLS handshake
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    
    return true;
}
```

### Step 4: Use SSL_read() and SSL_write()

Replace regular `read()` and `send()` calls with SSL versions:

```cpp
// Reading
char buffer[1024];
int bytes = SSL_read(ssl, buffer, sizeof(buffer));

// Writing
SSL_write(ssl, data, length);
```

### Step 5: Cleanup

```cpp
GeminiConnection::~GeminiConnection() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(socket_fd);
}
```

## Testing TLS Implementation

### Manual Testing with OpenSSL

```bash
# Start server with TLS
./protocol_server gemini 1965 --tls

# In another terminal, connect with openssl s_client
echo -e "gemini://localhost/\r" | openssl s_client -connect localhost:1965 -quiet
```

### Automated Testing

Run the Level 6 TLS tests:

```bash
# Run all TLS tests
pytest -v test_protocol_server.py -k "tls"

# Run specific TLS tests
pytest -v test_protocol_server.py::test_gemini_tls_connection
pytest -v test_protocol_server.py::test_gemini_tls_certificate_validation
pytest -v test_protocol_server.py::test_gemini_tls_encrypted_transfer
pytest -v test_protocol_server.py::test_gemini_tls_vs_non_tls
```

## Common Issues

### Issue 1: Certificate Not Found

**Error**: `Error loading certificate`

**Solution**: Make sure `cert.pem` and `key.pem` are in the same directory as the server binary.

### Issue 2: SSL Handshake Failed

**Error**: `SSL_accept() failed`

**Solution**: 
- Check that certificate and key match
- Verify certificate is valid
- Ensure client is using SSL/TLS connection

### Issue 3: Connection Refused

**Error**: Client can't connect

**Solution**:
- Verify server started with `--tls` flag
- Check that port is not blocked by firewall
- Ensure client is using SSL context

### Issue 4: Wrong Version Number

**Error**: `wrong version number`

**Solution**: Client is trying non-TLS connection to TLS server. Use SSL context in client.

## Grading

TLS implementation is worth **10% bonus points**:

- **2.5%** - SSL context initialization and certificate loading
- **2.5%** - Proper SSL handshake (SSL_accept)
- **2.5%** - Encrypted read/write operations
- **2.5%** - Proper cleanup and error handling

All 4 Level 6 tests must pass to receive full bonus points.

## Resources

- [OpenSSL Documentation](https://www.openssl.org/docs/)
- [OpenSSL SSL/TLS Programming](https://wiki.openssl.org/index.php/SSL/TLS_Client)
- [Gemini TLS Requirements](https://gemini.circumlunar.space/docs/specification.gmi)

## Example: Complete TLS Implementation

See the TODO comments in:
- `GeminiServer.h` - SSL context declaration
- `GeminiServer.cpp` - SSL initialization
- `GeminiConnection.h` - SSL per-connection
- `GeminiConnection.cpp` - SSL read/write operations

Follow the commented examples to implement TLS step by step.
