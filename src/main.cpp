#include <iostream>   // For printing
#include "socket.h"

int main() {

    try {
        int server_file_descriptor = startServer();
        test_accept(server_file_descriptor);
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        std::perror("server failed to start");
    }
}