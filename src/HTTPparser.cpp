#include "HTTPparser.h"
namespace fs = std::filesystem;


// ==================== HttpRequest ====================

const std::string& HttpRequest::getMethod() const
{
    return method;
}


const std::string& HttpRequest::getPath() const
{
    return path;
}


const std::vector<char>& HttpRequest::getBody() const
{
    return body;
}


int HttpRequest::parseRequest(
    const std::vector<char>& buffer,
    size_t& bytes_received,
    size_t& bytes_consumed)
{
    while (current_state != ParserState::Complete &&
           bytes_consumed < bytes_received)
    {
        switch (current_state)
        {
            case ParserState::Method:
            {
                if (buffer[bytes_consumed] != ' ')
                {
                    method.push_back(buffer[bytes_consumed]);
                }
                else
                {
                    current_state = ParserState::Path;
                }

                break;
            }

            case ParserState::Path:
            {
                if (buffer[bytes_consumed] != ' ')
                {
                    path.push_back(buffer[bytes_consumed]);
                }
                else
                {
                    current_state = ParserState::Version;
                }

                break;
            }
            // skips version from request buffer
            case ParserState::Version:
            {
                if (buffer[bytes_consumed] == '\n')
                {
                    current_state = ParserState::HeaderKey;
                }

                break;
            }

            case ParserState::HeaderKey:
            {
                if (buffer[bytes_consumed] == ':')
                {
                    // Header key is complete.
                    // Next state will parse its value.
                    current_state = ParserState::HeaderValue;
                    bytes_consumed++;
                }
                // Checks if the last four characters are "\r\n\r\n"
                // and, if so, reaches the end of headers.
                else if (buffer[bytes_consumed - 1] == '\r' &&
                         buffer[bytes_consumed] == '\n')
                {
                    // Checks for body.
                    if (headers.find("Content-Length") != headers.end())
                    {
                        body_length = std::stoi(headers["Content-Length"]);
                        current_state = ParserState::Body;
                    }
                    else
                    {
                        current_state = ParserState::Complete;
                    }
                }
                // Excludes '\r' and '\n' from the header key.
                else if (buffer[bytes_consumed] != '\r' &&
                         buffer[bytes_consumed] != '\n')
                {
                    header_key += buffer[bytes_consumed];
                }

                break;
            }

            case ParserState::HeaderValue:
            {
                // If statement to check for new line in HTTP headers.
                if (buffer[bytes_consumed - 1] == '\r' &&
                    buffer[bytes_consumed] == '\n')
                {
                    header_key.clear();
                    current_state = ParserState::HeaderKey;
                }
                // Parse the value for the header.
                else if (buffer[bytes_consumed] != '\r')
                {
                    headers[header_key] += buffer[bytes_consumed];
                }

                break;
            }

            // Parses the body for the HTTP request.
            case ParserState::Body:
            {
                body.push_back(buffer[bytes_consumed]);

                if (body.size() == body_length)
                {
                    body_length = 0;
                    current_state = ParserState::Complete;
                }

                break;
            }

            case ParserState::Complete:
            {
                break;
            }
        }

        bytes_consumed++;
    }

    return 0;
}

// testing function for the httpRequest class
void HttpRequest::printRequest() const
{
    std::cout << "----- HTTP Request -----\n";
    std::cout << "Method: " << method << '\n';
    std::cout << "Path: " << path << '\n';

    std::cout << "Headers:\n";

    for (const auto& [key, value] : headers)
    {
        std::cout << "  " << key << ": " << value << '\n';
    }

    std::cout << "Expected body length: " << body_length << '\n';
    std::cout << "Actual body length: " << body.size() << '\n';

    if (!body.empty())
    {
        std::cout << "Body: ";

        for (char c : body)
        {
            std::cout << c;
        }

        std::cout << '\n';
    }

    std::cout << "------------------------\n";
}


// ==================== HttpResponse ====================

std::vector<char> HttpResponse::handleGetRequest()
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        status_code = "404";
        message = "Not Found";
        return serializeResponse();
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    body.resize(size);
    file.read(body.data(), size);

    status_code = "200";
    message = "OK";
    headers["Content-Length"] = std::to_string(size);
    return serializeResponse();
}


std::vector<char> HttpResponse::handlePostRequest(const HttpRequest& request)
{
    std::ofstream file(path, std::ios::binary);

    if (!file)
    {
        status_code = "500";
        message = "Internal Server Error";
        return serializeResponse();
    }

    const auto& request_body = request.getBody();

    file.write(request_body.data(), request_body.size());

    if (!file)
    {
        status_code = "500";
        message = "Internal Server Error";
        return serializeResponse();
    }

    status_code = "200";
    message = "OK";
    return serializeResponse();
}


std::vector<char> HttpResponse::handlePutRequest(const HttpRequest& request)
{
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if (!file)
    {
        status_code = "500";
        message = "Internal Server Error";
        return serializeResponse();
    }

    const auto& request_body = request.getBody();

    file.write(request_body.data(), request_body.size());

    if (!file)
    {
        status_code = "500";
        message = "Internal Server Error";
        return serializeResponse();
    }

    status_code = "200";
    message = "OK";
    return serializeResponse();
}


std::vector<char> HttpResponse::handleDeleteRequest()
{
    if (!std::filesystem::exists(path))
    {
        status_code = "404";
        message = "Not Found";
        return serializeResponse();
    }

    if (!std::filesystem::remove(path))
    {
        status_code = "500";
        message = "Internal Server Error";
        return serializeResponse();
    }

    status_code = "204";
    message = "No Content";
    return serializeResponse();
}


HttpResponse::HttpResponse(std::string path)
{
    this->path = "www" + path;

    fs::path file_path(path);
    std::string extension = file_path.extension().string();

    auto it = content_types.find(extension);

    if (it != content_types.end())
    {
        headers["Content-Type"] = it->second;
    }
    else
    {
        headers["Content-Type"] = "application/octet-stream";
    }
}


std::vector<char> HttpResponse::handleRequest(HttpRequest& request)
{
    if (request.getMethod() == "GET")
        return handleGetRequest();

    if (request.getMethod() == "POST")
        return handlePostRequest(request);

    if (request.getMethod() == "PUT")
        return handlePutRequest(request);

    if (request.getMethod() == "DELETE")
        return handleDeleteRequest();

    status_code = "405;";
    message = "Method Not Allowed";
    return serializeResponse();
}


char* HttpResponse::parseResponse(
    std::vector<char>& send_buffer,
    size_t bytes_sent)
{
    return send_buffer.data() + bytes_sent;
}


std::vector<char> HttpResponse::serializeResponse()
{
    std::vector<char> resp;
    std::string temp;

    temp = "HTTP/1.1 " + status_code + " " + message + "\r\n";

    for (const auto& [key, value] : headers)
    {
        temp += key + ": " + value + "\r\n";
    }

    temp += "\r\n";

    resp.assign(temp.begin(), temp.end());

    resp.insert(resp.end(), body.begin(), body.end());

    return resp;
}


