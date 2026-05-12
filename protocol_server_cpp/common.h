#ifndef COMMON_H
#define COMMON_H

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

inline void errexit(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

// Utility functions for protocol implementations

// Get MIME type based on file extension
inline std::string get_mime_type(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    
    std::string ext = path.substr(dot_pos + 1);
    
    // Common MIME types
    if (ext == "txt") return "text/plain";
    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "gmi") return "text/gemini";
    if (ext == "md") return "text/markdown";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "png") return "image/png";
    if (ext == "gif") return "image/gif";
    if (ext == "pdf") return "application/pdf";
    if (ext == "json") return "application/json";
    if (ext == "xml") return "application/xml";
    if (ext == "zip") return "application/zip";
    
    return "application/octet-stream";
}

// Get Gopher item type based on file/directory
inline char get_gopher_type(const std::string& path, bool is_dir) {
    if (is_dir) {
        return '1';  // Directory
    }
    
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return '9';  // Binary file (default)
    }
    
    std::string ext = path.substr(dot_pos + 1);
    
    // Text files
    if (ext == "txt" || ext == "md" || ext == "log") return '0';
    // GIF images
    if (ext == "gif") return 'g';
    // Images (other)
    if (ext == "jpg" || ext == "jpeg" || ext == "png") return 'I';
    // HTML
    if (ext == "html" || ext == "htm") return 'h';
    // Binary
    return '9';
}

// Check if path is safe (no directory traversal)
inline bool is_safe_path(const std::string& path) {
    // Check for directory traversal attempts
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    // Path should start with / or be empty
    if (!path.empty() && path[0] != '/') {
        return false;
    }
    
    return true;
}

// Check if path is a directory
inline bool is_directory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

// Check if path is a regular file
inline bool is_regular_file(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISREG(st.st_mode);
    }
    return false;
}

// Get file size
inline long get_file_size(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return st.st_size;
    }
    return -1;
}

#endif
