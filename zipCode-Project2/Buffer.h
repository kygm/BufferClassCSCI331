#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <fstream>

class Buffer {
public:
    void pack(const std::string& data);
    std::string unpack();
    void readHeader(std::ifstream& file);
    void writeHeader(std::ofstream& file);

private:
    std::string buffer;
};

#endif