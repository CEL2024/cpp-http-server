#include <iostream>
#include <string>
#include <cstring>
#include <iostream>   // For printing
#include <vector>     // For the vector container
#include <algorithm>  // For sorting
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main () {

    // first print
    std::cout << "Socket programming in C++" << std::endl;

    // creates server socket
    int s_fd;
    s_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (s_fd == -1) {
        perror("socket");
        return 1;
    }
    std::cout << "socket() succeeded. fd = " << s_fd << std::endl;

    int port = 8080;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    

    // binds the socket to the port and address
    int bind_result = bind(s_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (bind_result == -1) {
        perror("bind");
        close(s_fd);
        return 1;
    }
    std::cout << "bind() succeeded" << std::endl;

    // listens for incoming connections on the server socket with a backlog of 5
    int listen_result = listen(s_fd, 5);
    

    if (listen_result == -1) {
        perror("listen");
        close(s_fd);
        return 1;
    }
    std::cout << "listening on port 8080" << std::endl;

    // creates client socket and accepts incoming connections
    struct sockaddr_in client_addr;
    socklen_t clientSize = sizeof(client_addr);

    int client_fd = accept(s_fd, (struct sockaddr*)&client_addr, &clientSize);
    
    if (client_fd == -1) {
        perror("accept");
        close(s_fd);
        return 1;
    }

    std::cout << "Client connected! client_fd = "
              << client_fd << std::endl;


    // creates buffer to store received data from client
    int buffer_size = 1024;
    std::vector<char> buffer(buffer_size);

    // while loop to continuously receive data from the client and send a response
    while (true) {

        // recieves data from client
        ssize_t received_bytes = recv(client_fd, buffer.data(), buffer.size(), 0);

        if (received_bytes == -1) {
            perror("recv");
            close(client_fd);
            close(s_fd);
            return 1;
        }
        else if (received_bytes == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Received " << received_bytes << " bytes from client: "
                << std::string(buffer.data(), received_bytes) << std::endl;

        const char* response = "message received. Hi!\n";

        ssize_t sent_bytes = send(
            client_fd,
            response,
            strlen(response),
            0
        );

        if (sent_bytes == -1) {
            perror("send");
        }

    }


    // closes connections and sockets
    close(client_fd);
    close(s_fd);

    return 0;

}