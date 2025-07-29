#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

#include "logger.hpp"

enum class Method {
    Get,
    Post,
    Put,
    Delete,
    Head,
    Options,
    Patch,
};

enum class ContentType {
    Html,
    Json,
    Xml,
    Plain,
    Css,
    JavaScript,
    Png,
    Jpeg,
    Gif,
    Svg,
    Pdf,
    Icon,
};

enum class Status {
    Ok,
    NotFound,
    BadRequest,
};

struct Endpoint {
    Method method;
    std::function<std::string(std::string endpoint)> handler;
    ContentType content_type;
};

class HttpServer {
public:
    HttpServer();
    HttpServer(bool log_to_console, bool log_to_file);
    HttpServer(std::string address);
    HttpServer(std::string address, bool log_to_console, bool log_to_file);
    HttpServer(std::string address, int port);
    HttpServer(
        std::string address,
        int port,
        bool log_to_console,
        bool log_to_file
    );
    HttpServer(
        std::string address,
        int port,
        int queue_size,
        bool log_to_console,
        bool log_to_file
    );

    ~HttpServer();

    void openBrowser();
    void route(
        std::string endpoint,
        Method method,
        std::function<std::string(std::string endpoint)> handler,
        ContentType content_type
    );
    void acceptClient();

private:
    std::string address;
    int port;
    int queue_size;
    Logger log;
    int server_socket;
    std::vector<std::map<std::string, Endpoint>> endpoints;

private:
    void prepareSocket();
    void handleClientRequest(int client_socket);
    std::map<std::string, std::string> parseHttpHeader(
        const std::string &request
    );
    std::string getContentTypeString(ContentType content_type);
    void closeSocket(int socket);
};
