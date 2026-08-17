#include <string>
#include <cstring>
#include <iostream>   // For printing
#include <vector>     // For the vector container

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "socket.h"
#include "HTTPparser.h"
namespace fs = std::filesystem;

// make server start function
// make close and cleanup function


int startServer () {

    // creates server socket
    int s_fd;
    s_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (s_fd == -1) {
        perror("socket");
        return 1;
    }

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

    // listens for incoming connections on the server socket with a backlog of 10
    int listen_result = listen(s_fd, 10);
    

    if (listen_result == -1) {
        perror("listen");
        close(s_fd);
        return 1;
    }
    std::cout << "listening on port 8080" << std::endl;

    return s_fd;
}


int test_accept(int s_fd) {

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
    int buffer_size = 4000;
    std::vector<char> buffer(buffer_size);

    // recv/send will be implemented into the worker thread not here
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

        // initializes the request object, parses the request, and includes a test command for linux.
        HttpRequest request;
        size_t var1 = buffer.size();
        size_t var2 = 0;


        
        request.parseRequest(buffer, var1, var2);
        request.printRequest();
        // printf 'POST /test HTTP/1.1\r\nHost: localhost\r\nContent-Length: 12\r\nContent-Type: text/plain\r\n\r\nHello World!' | nc localhost 8080
        HttpResponse r(request.getPath());
        std::vector<char>resp;
        r.handleRequest(request);

        // creates the response from the data within the request object and sends the response to the client.
        const char* response = r.parseResponse(resp, 0);

        ssize_t sent_bytes = send(
            client_fd,
            response,
            resp.size(),
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
