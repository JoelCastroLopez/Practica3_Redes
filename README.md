[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/P8TvcyZW)
# Implementing Gopher and Gemini Protocol Servers

This assignment is to be developed using C++ in Linux. Students must complete this starter code to implement both the **Gopher** (RFC 1436) and **Gemini** protocols.

## Protocols Overview

### Gopher Protocol (RFC 1436)
Gopher is a simple text-based protocol from the early 1990s. It uses a selector-based system where clients send a selector string and receive either a file or a menu of items.

**Key Features:**
- Simple request/response model
- Tab-delimited menu format
- Item types (0=text, 1=directory, 9=binary, etc.)
- No headers, just raw data transfer

### Gemini Protocol
Gemini is a modern lightweight protocol designed as a middle ground between Gopher and HTTP. It uses a URL-based request system and supports MIME types.

**Key Features:**
- URL-based requests (gemini://host/path)
- Status codes (20=success, 51=not found, etc.)
- MIME type support
- Gemtext format for hypertext documents
- Optional TLS support (advanced feature)

## Project Structure

The only files you need to edit are in the `protocol_server_cpp` directory:

```
protocol_server_cpp/
├── common.h                  # Utility functions (provided)
├── ProtocolServer.h/cpp      # Base server class
├── GopherServer.h/cpp        # Gopher server implementation
├── GopherConnection.h/cpp    # Gopher request handler
├── GeminiServer.h/cpp        # Gemini server implementation
├── GeminiConnection.h/cpp    # Gemini request handler
├── protocol_server.cpp       # Main entry point
└── CMakeLists.txt           # Build configuration
```

## What You Need to Implement

Look for `TODO` comments in the code. Key functions to implement:

### 1. Socket Operations (`ProtocolServer.cpp`)
- `define_socket_TCP()` - Create, bind, and listen on a TCP socket

### 2. Gopher Protocol (`GopherConnection.cpp`)
- `read_selector()` - Read selector from client (terminated by \r\n)
- `selector_to_path()` - Convert selector to filesystem path
- `send_file()` - Send file contents to client
- `send_directory()` - Generate and send Gopher menu

### 3. Gemini Protocol (`GeminiConnection.cpp`)
- `read_url()` - Read URL from client (max 1024 bytes)
- `parse_url_path()` - Extract path from gemini:// URL
- `send_file()` - Send file with Gemini header (status + MIME type)
- `send_directory()` - Generate gemtext directory listing

### 4. Optional: TLS Support (Advanced)
- Implement TLS encryption for Gemini using OpenSSL

## Compiling the Code

1. Change to the project directory:
```shell
cd protocol_server_cpp
```

2. Generate build files:
```shell
cmake .
```

3. Compile:
```shell
make
```

After compilation, a binary named `protocol_server` will be generated.

## Running the Server

### Gopher Server
```shell
./protocol_server gopher [port]
```

Examples:
```shell
./protocol_server gopher          # Default port 7070
./protocol_server gopher 7070     # Specific port
./protocol_server gopher 0        # Random port
```

### Gemini Server
```shell
./protocol_server gemini [port] [--tls]
```

Examples:
```shell
./protocol_server gemini          # Default port 1965, no TLS
./protocol_server gemini 1965     # Specific port
./protocol_server gemini 1965 --tls  # With TLS (advanced)
```

## Testing Your Implementation

### Manual Testing

#### Test Gopher with netcat:
```shell
# Request root directory
echo "/\r" | nc localhost 7070

# Request a specific file
echo "/test.txt\r" | nc localhost 7070

# Request subdirectory
echo "/docs/\r" | nc localhost 7070
```

#### Test Gemini with netcat:
```shell
# Request root
echo "gemini://localhost/\r" | nc localhost 1965

# Request specific file
echo "gemini://localhost/test.txt\r" | nc localhost 1965
```

### Testing with Real Clients

For a more comprehensive testing experience using actual Gopher and Gemini clients (Lynx, Lagrange, Amfora, Bombadillo, etc.), see **[TESTING.md](TESTING.md)**.

Real clients provide:
- Visual feedback and proper content rendering
- Interactive navigation and link following
- Stricter protocol compliance checking
- Better understanding of user experience

### Test Levels for Grading

The autograding system evaluates your implementation based on the following levels (Total: 100 points):

0. **Level 0: Repo Cleanliness** (5 pts): Verification that no binaries or temporary build files are uploaded.
1. **Level 1-2: Bases** (10 pts): Basic socket operations and basic Gopher protocol.
2. **Level 3: Gopher Advanced** (15 pts): Tab-delimited menus, binary files, and security (path traversal).
3. **Level 3.5: Gopher Sequential** (5 pts): Handling multiple requests in the same execution.
4. **Level 4: Gemini Basic** (15 pts): Basic Gemini protocol (URL parsing, success codes).
5. **Level 5: Gemini Advanced** (15 pts): Gemtext, MIME types, and security.
6. **Level 5.5: Gemini Sequential** (5 pts): Sequential requests in Gemini.
7. **Level 6: Gemini TLS** (15 pts): Implementation of TLS encryption (Optional/Advanced).
8. **Level 7-8: Advanced & Stress** (15 pts): Concurrency (multiple clients) and large file transfers.

## Protocol Specifications

### Gopher Request Format
```
<selector><CR><LF>
```

Example:
```
/docs/readme.txt\r\n
```

### Gopher Menu Format
```
<type><display_string><TAB><selector><TAB><host><TAB><port><CR><LF>
```

Example:
```
0README.txt	/README.txt	localhost	7070\r\n
1Documents	/docs	localhost	7070\r\n
.\r\n
```

### Gopher Item Types
- `0` - Text file
- `1` - Directory
- `3` - Error
- `9` - Binary file
- `g` - GIF image
- `I` - Image (other)
- `h` - HTML file

### Gemini Request Format
```
<URL><CR><LF>
```

Example:
```
gemini://localhost/index.gmi\r\n
```

### Gemini Response Format
```
<STATUS><SPACE><META><CR><LF>
<BODY>
```

Example:
```
20 text/gemini\r\n
# Welcome\n
=> /docs/ Documentation\n
```

### Gemini Status Codes
- `20` - Success
- `40` - Temporary failure
- `51` - Not found
- `59` - Bad request

### Gemtext Format
```
# Heading
## Subheading
=> /path/to/file Link text
* List item
> Quote
```

## Common Pitfalls

1. **Forgetting CR+LF**: Both protocols require `\r\n` line endings
2. **Buffer overflows**: Always check input lengths
3. **Path traversal**: Validate paths to prevent `../` attacks
4. **Binary vs text**: Handle binary files correctly (don't add line endings)
5. **Socket cleanup**: Always close sockets and file descriptors
6. **Thread safety**: Be careful with shared resources in multi-threaded code

## Useful Resources

- [RFC 1436 - Gopher Protocol](https://www.rfc-editor.org/rfc/rfc1436)
- [Gemini Protocol Specification](https://gemini.circumlunar.space/docs/specification.gmi)
- [Gopher Client: Lynx](http://lynx.invisible-island.net/)
- [Gemini Client: Lagrange](https://gmi.skyjake.fi/lagrange/)

## Submission Policy and Attempt Limits

To ensure responsible use of computing resources and encourage best development practices:

1. **Submission Branch**: Autograding tests will **only run on the `main` branch**. Pushes to other branches will not trigger automatic evaluation.
2. **Attempt Limit**: Each group repository has a maximum of **10 automatic autograding attempts** (each push to `main` counts as one attempt).
3. **Tracking**: You can check how many attempts you have used and how many are remaining in the **Summary** of each execution under the "Actions" tab in GitHub.
4. **Exceeding the Limit**: Once more than 10 attempts are used, the pipeline will fail automatically.

It is strongly recommended to **test locally** before pushing changes to `main`.

## Getting Help

- Check the TODO comments in the code
- Review the provided utility functions in `common.h`
- Test incrementally
- Use manual testing with netcat to debug issues
- Read the protocol specifications carefully

Good luck!
