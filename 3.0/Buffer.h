#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <fstream>

class Buffer {
public:
    /**
     * @brief Packs a string into a length-indicated format.
     *
     * Example: "Hello" becomes "5,Hello"
     *
     * @param data The string to pack.
     */
    void pack(const std::string& data);

    /**
     * @brief Unpacks the string (ignores the length indicator).
     *
     * @return The original string.
     */
    std::string unpack();

    /**
     * @brief Reads the header record from the input file stream.
     *
     * @param file The input stream.
     */
    void readHeader(std::ifstream& file);

    /**
     * @brief Writes the header record to the output file stream.
     *
     * @param file The output stream.
     */
    void writeHeader(std::ofstream& file);

    /**
     * @brief Returns the internal packed string.
     *
     * @return The packed string.
     */
    std::string getBuffer() const { return buffer; }

private:
    std::string buffer;
};

#endif
