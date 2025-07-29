#pragma once

#include <string>

class Logger {
public:
    Logger(bool to_console, bool to_file);
    void info(std::string message);
    void error(std::string message);

private:
    bool to_console;
    bool to_file;

private:
    std::string getDateTimeNow();
    void appendFile(std::string filePath, std::string message);
    void handleMessage(std::string message, std::string log_type);
};
