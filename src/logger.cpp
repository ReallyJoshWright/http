#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <chrono>
#include <print>
#include <ctime>

#include "logger.hpp"

namespace fs = std::filesystem;

using std::string;
using std::cerr;

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////
Logger::Logger(bool to_console, bool to_file) {
    this->to_console = to_console;
    this->to_file = to_file;
}

///////////////////////////////////////////////////////////////////////////////
// public methods
///////////////////////////////////////////////////////////////////////////////
void Logger::info(string message) {
    handleMessage(message, "INF");
}

void Logger::error(string message) {
    handleMessage(message, "ERR");
}

///////////////////////////////////////////////////////////////////////////////
// private methods
///////////////////////////////////////////////////////////////////////////////
string Logger::getDateTimeNow() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    localtime_r(&now_c, &local_time);

    std::ostringstream out;
    out << std::put_time(&local_time, "%m/%d/%Y %H:%M:%S");

    return out.str();
}

void Logger::appendFile(string file_path, string message) {
    std::ofstream file(file_path, std::ios::app);
    if (!file) {
        println(cerr, "Error opening file");
    }

    std::ostringstream out;
    out << message << "\n";
    file << out.str();
}

void Logger::handleMessage(string message, string log_type) {
    string now = getDateTimeNow();
    std::ostringstream out;
    out << "[" << now << " " << log_type << "]" << " " << message;
    string msg = out.str();

    if (to_console) {
        println("{}", msg);
    }

    if (to_file) {
        fs::path log_dir = "logs";
        fs::path log_filename = "tcp_log.txt";
        fs::path file_path = log_dir / log_filename;

        if (!fs::exists(log_dir)) {
            fs::create_directory(log_dir);
        }

        string file_path_str = file_path.string();
        appendFile(file_path_str, msg);
    }
}
