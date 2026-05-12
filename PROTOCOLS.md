# Protocol Specifications

This document provides detailed specifications for the Gopher and Gemini protocols that students need to implement.

## Gopher Protocol (RFC 1436)

### Overview
Gopher is a distributed document search and retrieval protocol designed in 1991. It predates the World Wide Web and uses a simple menu-based interface.

### Request Format
A Gopher request consists of a selector string followed by CR+LF:

```
<selector><CR><LF>
```

**Examples:**
- Root directory: `/\r\n` or just `\r\n`
- Specific file: `/docs/readme.txt\r\n`
- Subdirectory: `/images/\r\n`

**Important Notes:**
- Maximum selector length: typically 255 characters (be generous, allow up to 1024)
- Empty selector means root directory
- Selector should map to filesystem path

### Response Format

#### For Files
Simply send the file contents and close the connection. No headers, no metadata.

```
<file contents>
<connection closes>
```

#### For Directories (Menus)
Send a series of menu items, each on its own line, terminated by a period on a line by itself:

```
<type><display_text><TAB><selector><TAB><host><TAB><port><CR><LF>
<type><display_text><TAB><selector><TAB><host><TAB><port><CR><LF>
...
.<CR><LF>
```

**Example Menu:**
```
0README.txt	/README.txt	localhost	7070\r\n
1Documents	/docs	localhost	7070\r\n
9binary.zip	/files/binary.zip	localhost	7070\r\n
.\r\n
```

### Item Types

| Type | Description | Use Case |
|------|-------------|----------|
| `0` | Text file | .txt, .md, plain text |
| `1` | Directory/Menu | Directories, folders |
| `2` | CSO phone book | (rarely used) |
| `3` | Error | Error messages |
| `4` | BinHex file | (Mac-specific, rarely used) |
| `5` | DOS binary | (rarely used) |
| `6` | UUEncoded file | (rarely used) |
| `7` | Search query | (advanced feature) |
| `8` | Telnet session | (rarely used) |
| `9` | Binary file | .zip, .exe, .bin, etc. |
| `g` | GIF image | .gif files |
| `I` | Image file | .jpg, .png, other images |
| `h` | HTML file | .html, .htm |
| `i` | Informational | Non-selectable text in menus |
| `s` | Sound file | .mp3, .wav, etc. |

**For this assignment, focus on:** `0`, `1`, `3`, `9`, `g`, `I`, `h`

### Implementation Tips

1. **Selector Parsing:**
   ```cpp
   // Read until \r\n
   // Validate: no "..", check length
   // Map to filesystem: "/" -> ".", "/file.txt" -> "./file.txt"
   ```

2. **Item Type Detection:**
   ```cpp
   // Use file extension
   if (is_directory) return '1';
   if (ext == "txt") return '0';
   if (ext == "gif") return 'g';
   // ... etc
   return '9';  // default to binary
   ```

3. **Menu Generation:**
   ```cpp
   // For each file in directory:
   //   type = get_gopher_type(file)
   //   send: type + filename + \t + selector + \t + host + \t + port + \r\n
   // Send: ".\r\n"
   ```

4. **Error Handling:**
   ```cpp
   // Send error item:
   "3File not found\terror\terror\t0\r\n"
   ```

---

## Gemini Protocol

### Overview
Gemini is a modern protocol designed in 2019 as a lightweight alternative to HTTP. It's heavier than Gopher but simpler than HTTP.

### Request Format
A Gemini request is a URL followed by CR+LF:

```
<URL><CR><LF>
```

**Examples:**
- `gemini://localhost/\r\n`
- `gemini://example.com/docs/file.gmi\r\n`
- `gemini://example.com:1965/index.gmi\r\n`

**Important Constraints:**
- Maximum URL length: **1024 bytes** (including CR+LF)
- Must start with `gemini://`
- Must reject URLs longer than 1024 bytes with status `59`

### Response Format
All responses start with a status line, followed by optional body:

```
<STATUS><SPACE><META><CR><LF>
<BODY>
```

**Examples:**

Success:
```
20 text/gemini\r\n
# Welcome to Gemini\n
=> /docs/ Documentation\n
```

Not found:
```
51 Not found\r\n
```

Bad request:
```
59 Bad request\r\n
```

### Status Codes

| Code | Meaning | When to Use |
|------|---------|-------------|
| `10` | INPUT | Requesting input from user (advanced) |
| `20` | SUCCESS | Request successful, body follows |
| `30` | REDIRECT - TEMPORARY | Temporary redirect (advanced) |
| `31` | REDIRECT - PERMANENT | Permanent redirect (advanced) |
| `40` | TEMPORARY FAILURE | Generic temporary error |
| `41` | SERVER UNAVAILABLE | Server is down/overloaded |
| `42` | CGI ERROR | CGI script failed (advanced) |
| `43` | PROXY ERROR | Proxy request failed (advanced) |
| `44` | SLOW DOWN | Rate limiting (advanced) |
| `50` | PERMANENT FAILURE | Generic permanent error |
| `51` | NOT FOUND | File/resource not found |
| `52` | GONE | Resource permanently removed |
| `53` | PROXY REQUEST REFUSED | Won't proxy (advanced) |
| `59` | BAD REQUEST | Malformed request |
| `60` | CLIENT CERTIFICATE REQUIRED | TLS client cert needed (advanced) |
| `61` | CERTIFICATE NOT AUTHORIZED | Invalid client cert (advanced) |
| `62` | CERTIFICATE NOT VALID | Expired/invalid cert (advanced) |

**For this assignment, focus on:** `20`, `40`, `51`, `59`

### Meta Field

For status `20` (success), the meta field contains the MIME type:
- `text/plain` - Plain text files
- `text/gemini` - Gemtext files (.gmi)
- `text/html` - HTML files
- `image/jpeg` - JPEG images
- `image/png` - PNG images
- `application/octet-stream` - Binary files

For errors, the meta field contains a human-readable message:
- `51 Not found`
- `59 Bad request`
- `40 Server error`

### Gemtext Format

Gemtext is Gemini's native hypertext format. It's line-oriented and simple:

```
# Heading (level 1)
## Heading (level 2)
### Heading (level 3)

Regular paragraph text.

=> /path/to/resource Link text
=> gemini://example.com/ External link

* List item 1
* List item 2

> This is a quote
> It can span multiple lines

```
Preformatted text
Code or ASCII art
```

**For directory listings, use:**
```
# Directory Listing

=> /file1.txt file1.txt
=> /file2.gmi file2.gmi
=> /subdir/ subdir/
```

### URL Parsing

Students need to extract the path from a Gemini URL:

```
gemini://hostname:port/path/to/resource
         ^^^^^^^^ ^^^^  ^^^^^^^^^^^^^^^^^
         host     port  path
```

**Examples:**
- `gemini://localhost/` → path: `/`
- `gemini://localhost/file.txt` → path: `/file.txt`
- `gemini://localhost:1965/docs/index.gmi` → path: `/docs/index.gmi`

**Algorithm:**
1. Check URL starts with `gemini://`
2. Find first `/` after `gemini://`
3. Everything after that `/` is the path
4. If no `/` found, path is `/`

### Implementation Tips

1. **URL Reading:**
   ```cpp
   // Read until \r\n
   // Check length <= 1024
   // If too long: return "59 URL too long\r\n"
   ```

2. **URL Parsing:**
   ```cpp
   // Check starts with "gemini://"
   // Find first "/" after "gemini://"
   // Extract path
   // Validate path (no "..")
   ```

3. **Response Headers:**
   ```cpp
   // Success:
   string mime = get_mime_type(path);
   send("20 " + mime + "\r\n");
   
   // Error:
   send("51 Not found\r\n");
   ```

4. **Directory Listings:**
   ```cpp
   send("20 text/gemini\r\n");
   send("# Directory Listing\n\n");
   for (each file) {
       send("=> /" + path + "/" + filename + " " + filename + "\n");
   }
   ```

### TLS Support (Optional/Advanced)

The Gemini specification requires TLS, but for educational purposes, this is optional:

**Basic (No TLS):**
- Use regular TCP sockets
- Easier to implement and test
- Good for learning the protocol

**Advanced (With TLS):**
- Use OpenSSL library
- Generate self-signed certificate
- Wrap socket with SSL_accept()
- More realistic but complex

**Implementation:**
```cpp
// Link with: -lssl -lcrypto
#include <openssl/ssl.h>
#include <openssl/err.h>

// Initialize SSL context
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);

// For each connection:
SSL *ssl = SSL_new(ctx);
SSL_set_fd(ssl, socket_fd);
SSL_accept(ssl);

// Read/write using SSL_read() and SSL_write()
```

---

## Security Considerations

### Path Traversal Prevention

Both protocols must prevent directory traversal attacks:

**Bad Requests:**
- `/../../../etc/passwd`
- `/docs/../../secret.txt`
- `gemini://localhost/../config`

**Prevention:**
```cpp
bool is_safe_path(const string& path) {
    // Reject if contains ".."
    if (path.find("..") != string::npos) {
        return false;
    }
    
    // Ensure starts with / or is empty
    if (!path.empty() && path[0] != '/') {
        return false;
    }
    
    return true;
}
```

### Input Validation

1. **Check lengths:**
   - Gopher selector: < 1024 bytes
   - Gemini URL: ≤ 1024 bytes

2. **Validate format:**
   - Gopher: ends with \r\n
   - Gemini: starts with gemini://, ends with \r\n

3. **Sanitize paths:**
   - No ".."
   - No absolute paths outside server root
   - Check file exists before serving

### Resource Limits

1. **File size:** Consider limiting max file size
2. **Connections:** Limit concurrent connections
3. **Timeouts:** Set read/write timeouts
4. **Rate limiting:** Prevent abuse (advanced)

---

## Testing Checklist

### Gopher
- [ ] Serve text file
- [ ] Serve binary file
- [ ] Generate directory menu
- [ ] Handle subdirectories
- [ ] Return error for missing files
- [ ] Prevent path traversal
- [ ] Handle concurrent connections
- [ ] Correct item types

### Gemini
- [ ] Parse gemini:// URLs
- [ ] Return 20 status with MIME type
- [ ] Serve text files
- [ ] Serve binary files
- [ ] Generate gemtext directory listing
- [ ] Return 51 for missing files
- [ ] Return 59 for bad requests
- [ ] Reject URLs > 1024 bytes
- [ ] Prevent path traversal
- [ ] Handle concurrent connections
- [ ] (Optional) TLS encryption

---

## Example Implementations

### Gopher Menu Example

Request:
```
/\r\n
```

Response:
```
iWelcome to my Gopher server	fake	fake	0\r\n
i	fake	fake	0\r\n
0README.txt	/README.txt	localhost	7070\r\n
1Documents	/docs	localhost	7070\r\n
9download.zip	/files/download.zip	localhost	7070\r\n
.\r\n
```

### Gemini Directory Example

Request:
```
gemini://localhost/\r\n
```

Response:
```
20 text/gemini\r\n
# Directory Listing

=> /README.txt README.txt
=> /index.gmi index.gmi
=> /docs/ docs/
```

---

## References

- [RFC 1436 - The Internet Gopher Protocol](https://www.rfc-editor.org/rfc/rfc1436)
- [Gemini Protocol Specification](https://gemini.circumlunar.space/docs/specification.gmi)
- [Gopher Wikipedia](https://en.wikipedia.org/wiki/Gopher_(protocol))
- [Gemini FAQ](https://gemini.circumlunar.space/docs/faq.gmi)
