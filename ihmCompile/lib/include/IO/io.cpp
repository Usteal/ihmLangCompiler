#include "io.h"
#include <iostream>
#include <string>

void print(const std::string& msg) {
    std::cout << msg << std::endl;
}

std::string input(const std::string& prompt) {
    if (!prompt.empty()) std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}