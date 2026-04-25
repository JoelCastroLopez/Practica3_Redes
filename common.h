#pragma once
#include <string>
#include <map>

// Tipos MIME por extensión
static const std::map<std::string, std::string> MIME_TYPES = {
    {".txt",  "text/plain"},
    {".md",   "text/plain"},
    {".gmi",  "text/gemini"},
    {".html", "text/html"},
    {".htm",  "text/html"},
    {".pdf",  "application/pdf"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png",  "image/png"},
    {".gif",  "image/gif"},
    {".zip",  "application/zip"},
    {".bin",  "application/octet-stream"},
};

// Tipos Gopher por extensión
static char gopher_type_for(const std::string& name) {
    auto dot = name.rfind('.');
    if (dot == std::string::npos) return '9';
    std::string ext = name.substr(dot);
    if (ext == ".txt" || ext == ".md")  return '0';
    if (ext == ".html"|| ext == ".htm") return 'h';
    if (ext == ".gif")                  return 'g';
    if (ext == ".jpg" || ext == ".jpeg"|| ext == ".png") return 'I';
    return '9'; // binario por defecto
}

// Tipo MIME para un fichero
static std::string mime_for(const std::string& name) {
    auto dot = name.rfind('.');
    if (dot != std::string::npos) {
        auto it = MIME_TYPES.find(name.substr(dot));
        if (it != MIME_TYPES.end()) return it->second;
    }
    return "application/octet-stream";
}

// Validación de path
static bool is_safe_path(const std::string& path) {
    if (path.find("..") != std::string::npos) return false;
    if (!path.empty() && path[0] != '/')       return false;
    return true;
}
