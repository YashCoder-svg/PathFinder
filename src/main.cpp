// =============================================================================
// main.cpp — Entry Point for PathFinder Pro
// =============================================================================

#include <iostream>
#include <string>
#include "server/Server.hpp"

int main(int argc, char* argv[]) {
    int port = 8080;
    std::string webDir = "./web";

    if (argc > 1) {
        port = std::stoi(argv[1]);
    }
    if (argc > 2) {
        webDir = argv[2];
    }

    std::cout << "Starting PathFinder Pro Backend...\n";
    Server server(port, webDir);
    server.start();

    return 0;
}
