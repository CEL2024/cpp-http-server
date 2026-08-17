#include <iostream>
#include <string>
#include <cstring>
#include <iostream>   // For printing
#include <vector> 
#include <filesystem>
#include <fstream>
#include <unordered_map>

enum class ParserState {
    Method,
    Path,
    Version,
    HeaderKey,
    HeaderValue,
    Body,
    Complete
};


class HttpRequest
{
private:
    std::string method;
    std::string path;
    std::vector<char> body;
    std::string header_key;
    std::unordered_map<std::string, std::string> headers;

    size_t body_length = 0;
    ParserState current_state = ParserState::Method;

public:
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::vector<char>& getBody() const;

    int parseRequest(
        const std::vector<char>& buffer,
        size_t& bytes_received,
        size_t& bytes_consumed
    );
    // for testing
    void printRequest() const;
};


class HttpResponse
{
private:
    // Http response variables
    std::string status_code;
    std::string path;
    std::string message;
    std::unordered_map<std::string, std::string> headers;
    std::vector<char> body;

    // Http method functions to populate http response object
    std::vector<char> handleGetRequest();
    std::vector<char> handlePostRequest(const HttpRequest& request);
    std::vector<char> handlePutRequest(const HttpRequest& request);
    std::vector<char> handleDeleteRequest();

    // creates send_buffer from Http Response object
    std::vector<char> serializeResponse();

public:
    // constructor for class
    HttpResponse(std::string path);

    std::vector<char> handleRequest(HttpRequest& request);

    // parses response and accounts for TCP stream while sending
    char* parseResponse(
        std::vector<char>& send_buffer,
        size_t bytes_sent
    );

    
};



const std::unordered_map<std::string, std::string> content_types = {
    {".html", "text/html"},
    {".htm",  "text/html"},
    {".txt",  "text/plain"},
    {".css",  "text/css"},
    {".js",   "text/javascript"},

    {".json", "application/json"},
    {".xml",  "application/xml"},
    {".pdf",  "application/pdf"},

    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif",  "image/gif"},
    {".svg",  "image/svg+xml"},
    {".ico",  "image/x-icon"},

    {".mp3",  "audio/mpeg"},
    {".mp4",  "video/mp4"},

    {".zip",  "application/zip"}
};




