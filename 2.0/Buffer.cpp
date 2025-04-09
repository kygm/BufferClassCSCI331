#include "Buffer.h"
#include <iostream>

/**
 * @brief Packs a string into a length-indicated format.
 *
 * Converts the input string into a format where the first part of the packed
 * data indicates the string's length, followed by the actual string.
 *
 * Example:
 * Input:  "Hello"
 * Output: "5,Hello"
 *
 * @param data The string to be packed.
 */
void Buffer::pack(const std::string &data)
{
    buffer = std::to_string(data.size()) + "," + data;
}

/**
 * @brief Unpacks a length-indicated string from the buffer.
 *
 * Extracts the string from a buffer containing a length-indicated format.
 * The length is used to validate the extracted substring.
 *
 * Example:
 * Input Buffer: "5,Hello"
 * Output: "Hello"
 *
 * @return The unpacked string.
 */
std::string Buffer::unpack()
{
    size_t commaPos = buffer.find(',');                    // Find position of the comma delimiter
    size_t length = std::stoi(buffer.substr(0, commaPos)); // Extract the length
    return buffer.substr(commaPos + 1, length);            // Extract and return the string
}

/**
 * @brief Reads the header record from the input file stream.
 *
 * Reads a single line from the provided input file stream and stores it in the buffer.
 * This function is used to process the header information of a file.
 *
 * @param file An input file stream object from which the header is read.
 */
void Buffer::readHeader(std::ifstream &file)
{
    std::getline(file, buffer);
}

/**
 * @brief Writes the header record to the output file stream.
 *
 * Writes the content of the buffer, typically containing a header, to the output file stream.
 *
 * @param file An output file stream object to which the header is written.
 */
void Buffer::writeHeader(std::ofstream &file)
{
    file << buffer << std::endl;
}