#pragma once
#include <string>

//compression algorithms
namespace CA {
    void RLEcompress(std::string source, std::string outputName);
    void RLEunpack(std::string source, std::string outputName);
}
