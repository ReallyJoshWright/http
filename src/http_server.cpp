#include <functional>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <format>
#include <string>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "http_server.hpp"
#include "logger.hpp"

using std::function;
using std::format;
using std::string;
using std::map;

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////
HttpServer::HttpServer() : log(false, false) {
    address = "127.0.0.1";
    port = 3000;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(bool log_to_console, bool log_to_file)
    : log(log_to_console, log_to_file) {
    address = "127.0.0.1";
    port = 3000;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(string address) : log(false, false) {
    if (address == "localhost") {
        this->address = "127.0.0.1";
    } else {
        this->address = address;
    }
    port = 3000;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(string address, bool log_to_console, bool log_to_file)
    : log(log_to_console, log_to_file) {
    if (address == "localhost") {
        this->address = "127.0.0.1";
    } else {
        this->address = address;
    }
    port = 3000;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(string address, int port) : log(false, false) {
    if (address == "localhost") {
        this->address = "127.0.0.1";
    } else {
        this->address = address;
    }
    this->port = port;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(
    string address,
    int port,
    bool log_to_console,
    bool log_to_file
) : log(log_to_console, log_to_file) {
    if (address == "localhost") {
        this->address = "127.0.0.1";
    } else {
        this->address = address;
    }
    this->port = port;
    queue_size = 10;
    server_socket = -1;

    prepareSocket();
}

HttpServer::HttpServer(
    string address,
    int port,
    int queue_size,
    bool log_to_console,
    bool log_to_file
) : log(log_to_console, log_to_file) {
    if (address == "localhost") {
        this->address = "127.0.0.1";
    } else {
        this->address = address;
    }
    this->port = port;
    this->queue_size = queue_size;
    server_socket = -1;

    prepareSocket();
}

///////////////////////////////////////////////////////////////////////////////
// destructors
///////////////////////////////////////////////////////////////////////////////
HttpServer::~HttpServer() {
    closeSocket(server_socket);
}

///////////////////////////////////////////////////////////////////////////////
// public member functions
///////////////////////////////////////////////////////////////////////////////
void HttpServer::openBrowser() {
    string cmd_str = format("xdg-open http://{}:{}", address, port);
    const char *cmd = cmd_str.c_str();
    std::system(cmd);
}

void HttpServer::route(
    string endpoint,
    Method method,
    function<string(string endpoint)> handler,
    ContentType content_type
) {
    Endpoint end;
    end.method = method;
    end.handler = handler;
    end.content_type = content_type;

    map<string, Endpoint> current_map;
    current_map[endpoint] = end;
    endpoints.push_back(current_map);
}

void HttpServer::serveDir(string directory) {
    string html;
    std::ifstream file("./public/index.html");
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        html = ss.str();
        file.close();
    } else {
        return "";
    }
}

void HttpServer::acceptClient() {
    log.info(format("Server listening on {}:{}...", address, port));

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int res = getsockname(server_socket, (struct sockaddr *)&addr, &addrlen);
    if (res != 0) {
        log.error(format(
            "Error retrieving server address with getsockname(): {}",
            strerror(errno)
        ));
        return;
    }

    while (true) {
        int client_socket = accept(
            server_socket,
            (struct sockaddr *)&addr,
            &addrlen
        );

        if (client_socket < 0) {
            log.error("Accept failed");
            continue;
        }

        log.info(format("Client socket created: {}", client_socket));

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        log.info(format(
            "Connection accepted from {}:{}",
            client_ip,
            ntohs(addr.sin_port)
        ));

        handleClientRequest(client_socket);
    }
}

///////////////////////////////////////////////////////////////////////////////
// private member functions
///////////////////////////////////////////////////////////////////////////////
void HttpServer::prepareSocket() {
    struct sockaddr_in addr;
    int enable = 1;

    int res1 = server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (res1 < 0) {
        log.error("Socket creation error");
        return;
    }

    log.info(format("Server socket created: {}", server_socket));

    int res2 = setsockopt(
        server_socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const char *)&enable,
        sizeof(enable)
    );

    if (res2 < 0) {
        log.error("setsockopt failed");
        closeSocket(server_socket);
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int res3 = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (res3 < 0) {
        log.error("Bind failed. Check if port is already in use");
        closeSocket(server_socket);
        return;
    }

    int res4 = listen(server_socket, queue_size);
    if (res4 < 0) {
        log.error("Listen failed");
        closeSocket(server_socket);
        return;
    }
}

void HttpServer::handleClientRequest(int client_socket) {
    char buffer[4096] = {0};
    int bytes_received;
    bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_received <= 0) {
        log.error(format(
            "Client {} disconnected during initial HTTP request "
            "(recv returned {})",
            client_socket, bytes_received
        ));

        closeSocket(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';
    string request(buffer);

    log.info(format(
        "Received request from client {}: {}",
        client_socket,
        request.substr(0, request.find('\n'))
    ));

    auto header = parseHttpHeader(request);

    string method_line = header["Method"];
    Method method;
    if (method_line.starts_with("GET")) {
        method = Method::Get;
    } else if (method_line.starts_with("POST")) {
        method = Method::Post;
    } else if (method_line.starts_with("PUT")) {
        method = Method::Put;
    } else if (method_line.starts_with("DELETE")) {
        method = Method::Delete;
    } else if (method_line.starts_with("HEAD")) {
        method = Method::Head;
    } else if (method_line.starts_with("OPTIONS")) {
        method = Method::Options;
    } else if (method_line.starts_with("PATCH")) {
        method = Method::Patch;
    }

    string response;
    size_t first_space = method_line.find(' ');
    if (first_space == string::npos) {
        response = "";
    }
    size_t second_space = method_line.find(' ', first_space + 1);
    if (second_space == string::npos) {
        response = "";
    }
    string endpoint = method_line
        .substr(first_space + 1, second_space - (first_space + 1));
    string content_type_str;
    ContentType content_type;
    Endpoint end;

    bool ok = false;
    for (auto endp : endpoints) {
        if (endp.count(endpoint) > 0) {
            if (method == endp[endpoint].method) {
                response = endp[endpoint].handler(endpoint);
                content_type = endp[endpoint].content_type;
                ok = true;
                break;
            }
        }
    }

    content_type_str = getContentTypeString(content_type);
    string response_header;

    if (ok) {
        response_header = format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{}", content_type_str, response.length(), response
        );

        write(client_socket, response_header.c_str(), response_header.length());
    } else if (response == "") {
        string response_header =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\r\n"
            "Content-Length: 13\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found";

        write(client_socket, response_header.c_str(), response_header.length());
        closeSocket(client_socket);
    } else {
        string response_header =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 15\r\n"
            "Connection: close\r\n"
            "\r\n"
            "400 Bad Request";

        write(client_socket, response_header.c_str(), response_header.length());
        closeSocket(client_socket);
    }
}

map<string, string> HttpServer::parseHttpHeader(const string &request) {
    map<string, string> header;
    std::istringstream iss(request);
    string line;

    string method = "Method";
    std::getline(iss, line);
    header[method] = line;

    while (std::getline(iss, line) && !line.empty() && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 1);

            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            header[key] = value;
        }
    }

    return header;
}

string HttpServer::getContentTypeString(ContentType content_type) {
    string return_type;
    if (content_type == ContentType::Html) {
        return_type = "text/html";
    } else if (content_type == ContentType::Json) {
        return_type = "application/json";
    } else if (content_type == ContentType::Xml) {
        return_type = "application/xml";
    } else if (content_type == ContentType::Plain) {
        return_type = "text/plain";
    } else if (content_type == ContentType::Css) {
        return_type = "text/css";
    } else if (content_type == ContentType::JavaScript) {
        return_type = "application/javascript";
    } else if (content_type == ContentType::Png) {
        return_type = "image/png";
    } else if (content_type == ContentType::Jpeg) {
        return_type = "image/jpeg";
    } else if (content_type == ContentType::Gif) {
        return_type = "image/gif";
    } else if (content_type == ContentType::Svg) {
        return_type = "image/svg+xml";
    } else if (content_type == ContentType::Pdf) {
        return_type = "application/pdf";
    } else if (content_type == ContentType::Icon) {
        return_type = "image/x-icon";
    }

    return return_type;
}

void HttpServer::closeSocket(int socket) {
    if (socket >= 0) {
        close(socket);
        log.info(format("Socket closed: {}", socket));
    }
}
