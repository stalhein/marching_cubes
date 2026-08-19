#include "logger.hpp"

#include <iostream>
#include <unistd.h>

constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* GREEN = "\033[32m";
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
    if (useColour) std::cout << GREEN << "[INFO] " << RESET;
    else std::cout << "[INFO] ";
    std::cout << message << '\n';
}

void Logger::debug(const std::string &message) {
    if (!verbose) return;
    if (useColour) std::cout << GREY << "[DEBUG] " << RESET;
    else std::cout << "[DEBUG] ";
    std::cout << message << '\n';
}

void Logger::warning(const std::string &message) {
    if (useColour) std::cout << YELLOW << "[WARNING] " << RESET;
    else std::cout << "[WARNING] ";
    std::cout << message << '\n';
}

void Logger::error(const std::string &message) {
    if (useColour) std::cout << RED << "[INFO] " << RESET;
    else std::cout << "[INFO] ";
    std::cout << message << '\n';
}
