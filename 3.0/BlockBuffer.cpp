#include "BlockBuffer.h"
#include "Buffer.h"
#include <fstream>
#include <sstream>

bool BlockBuffer::writeBlocks(const std::string &filename, const std::vector<Block> &blocks) {
    std::ofstream ofs(filename);
    if (!ofs)
        return false;

    // Write a file header using Buffer.
    Buffer headerBuffer;
    std::string header = "BSS,1.0,HEADER_SIZE,BlockSize,NumBlocks";
    headerBuffer.pack(header);
    ofs << headerBuffer.getBuffer() << "\n";

    // Write number of blocks.
    ofs << blocks.size() << "\n";

    // For each block, write its packed data preceded by its length.
    for (const auto &blk : blocks) {
        std::string blockData = blk.serialize();
        Buffer buf;
        buf.pack(blockData);
        std::string packedBlock = buf.getBuffer();
        ofs << packedBlock.size() << "\n";  // length indicator
        ofs << packedBlock << "\n";
    }
    ofs.close();
    return true;
}

bool BlockBuffer::readBlocks(const std::string &filename, std::vector<Block> &blocks) {
    std::ifstream ifs(filename);
    if (!ifs)
        return false;

    std::string headerLine;
    std::getline(ifs, headerLine); // Read and ignore header details.

    std::string blockCountStr;
    std::getline(ifs, blockCountStr);
    int blockCount = std::stoi(blockCountStr);
    for (int i = 0; i < blockCount; i++) {
        std::string lengthStr;
        if (!std::getline(ifs, lengthStr))
            break;
        int length = std::stoi(lengthStr);
        char *buffer = new char[length + 1];
        ifs.read(buffer, length);
        buffer[length] = '\0';
        std::string packedBlock(buffer);
        delete[] buffer;

        // Unpack the block.
        size_t commaPos = packedBlock.find(',');
        if (commaPos == std::string::npos)
            continue;
        int dataLength = std::stoi(packedBlock.substr(0, commaPos));
        std::string blockData = packedBlock.substr(commaPos + 1, dataLength);
        Block blk = Block::deserialize(blockData);
        blocks.push_back(blk);

        // Consume the newline after block data.
        std::string dummy;
        std::getline(ifs, dummy);
    }
    return true;
}
