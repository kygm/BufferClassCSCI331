#include "Buffer.h"
#include <iostream>

void Buffer::pack(const std::string& data) {
    buffer = std::to_string(data.size()) + "," + data;
}

std::string Buffer::unpack() {
    size_t commaPos = buffer.find(',');
    size_t length = std::stoi(buffer.substr(0, commaPos));
    return buffer.substr(commaPos + 1, length);
}

void Buffer::readHeader(std::ifstream& file) {
    std::getline(file, buffer);
}

void Buffer::writeHeader(std::ofstream& file) {
    file << buffer << std::endl;
}