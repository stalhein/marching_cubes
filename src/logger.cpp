#include "logger.hpp"

#include <iostream>
#include <unistd.h>

bool Logger::verbose = false;



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
