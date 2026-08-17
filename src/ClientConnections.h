#include <iostream>
#include <string>
#include <cstring>
#include <iostream>   // For printing
#include <vector> 
#include <unordered_map>
#include "HTTPparser.h"


class ClientConnections {

    private:
        size_t bytes_consumed = 0;
        size_t bytes_received = 0;

        HttpRequest request;
        HttpResponse response;

        bool keep_alive = false;

        size_t bytes_sent = 0;
        std::vector<char> send_buffer;

    public:

};