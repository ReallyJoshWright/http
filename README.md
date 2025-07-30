# HTTP

This is an http library in C++ from scratch. It will contain server and client
capabilities.

## Dependencies
- Linux
- make
- gcc with c++23 support

## Examples
```c++

#include <sstream>
#include <fstream>
#include <string>
#include <print>

#include "http_server.hpp"

using std::println;
using std::string;

string handler(string endpoint) {
    println("{}", endpoint);
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
    return html;
}

string handlerCss(string endpoint) {
    println("{}", endpoint);
    string css;
    std::ifstream file("./public/css/app.css");
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        css = ss.str();
        file.close();
    } else {
        return "";
    }
    return css;
}

string handlerJs(string endpoint) {
    println("{}", endpoint);
    string js;
    std::ifstream file("./public/js/main.js");
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        js = ss.str();
        file.close();
    } else {
        return "";
    }
    return js;
}

int main() {
    HttpServer http_server("127.0.0.1", 3000, 10, true, false);
    http_server.route("/", Method::Get, handler, ContentType::Html);
    http_server.route("/css/app.css", Method::Get, handlerCss, ContentType::Css);
    http_server.route("/js/main.js", Method::Get, handlerJs, ContentType::JavaScript);
    http_server.openBrowser();
    http_server.acceptClient();

    return 0;
}

```

## Documentation
- [websockets](./documentation/websockets.md)

## Todo
- add default directory loading for static files
- finish http protocol implementation
- add asynchronous options and epoll

- add websockets and file monitor for hot reloading
- add typescript converter (with its own file monitoring)

- add windows support
- add http client
- add https support
- add certbot functionality
- templating for dynamic content
