#include "Buffer.h"
#include <iostream>

/**
 * @brief Packs a string into a length-indicated format.
 */
void Buffer::pack(const std::string &data)
{
    buffer = std::to_string(data.size()) + "," + data;
}

/**
 * @brief Unpacks a length-indicated string from the buffer.
 */
std::string Buffer::unpack()
{
    size_t commaPos = buffer.find(',');
    size_t length = std::stoi(buffer.substr(0, commaPos));
    return buffer.substr(commaPos + 1, length);
}

/**
 * @brief Reads the header record from the input file stream.
 */
void Buffer::readHeader(std::ifstream &file)
{
    std::getline(file, buffer);
}

/**
 * @brief Writes the header record to the output file stream.
 */
void Buffer::writeHeader(std::ofstream &file)
{
    file << buffer << std::endl;
}
