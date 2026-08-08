#pragma once

// Fix Windows version definition for httplib on MinGW
#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

// =============================================================================
// Server.hpp — Web Server wrapping cpp-httplib & REST API
// =============================================================================

#include <string>

class Server {
public:
    explicit Server(int port = 8080, const std::string& staticDir = "./web");
    void start();

private:
    int port_;
    std::string staticDir_;
};
