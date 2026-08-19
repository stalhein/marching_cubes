#include "logger.hpp"

#include <iostream>
#include <unistd.h>

constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* GREY = "\033[90m";

bool Logger::verbose = false;

bool Logger::useColour = [] {
    if (std::getenv("NO_COLOR") != nullptr) return false;

    return (bool)(isatty(STDOUT_FILENO));
}();

void Logger::setVerbose(bool enabled) {
    verbose = enabled;
}

void Logger::info(const std::string &message) {
    std::cout << "[INFO] " << message << '\n';
}

void Logger::debug(const std::string &message) {
    if (!verbose) return;
    std::cout << "[DEBUG] " << message << '\n';
}

void Logger::warning(const std::string &message) {
    std::cout << "[WARNING] " << message << '\n';
}

void Logger::error(const std::string &message) {
    std::cout << "[ERROR] " << message << '\n';
}
