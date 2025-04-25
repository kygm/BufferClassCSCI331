#include "BlockBuffer.h"
#include "Buffer.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Writes a blocked sequence set file.
 *
 * Writes blocks to a file, including a header that specifies the format and the
 * number of blocks in the file. Each block is serialized, packed using Buffer,
 * and written alongside its size indicator.
 *
 * @param filename The name of the output file to write the blocks to.
 * @param blocks A vector of Block objects to write to the file.
 * @return True if the operation succeeds, false otherwise.
 */
bool BlockBuffer::writeBlocks(const std::string &filename, const std::vector<Block> &blocks)
{
    std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
    if (!ofs)
    {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return false;
    }

    // Write a file header
    Buffer headerBuffer;
    std::string header = "BSS,1.0," + std::to_string(blocks.size()); // Format: BSS,version,numBlocks
    headerBuffer.pack(header);
    ofs << headerBuffer.getBuffer() << "\n";

    // Write number of blocks
    ofs << blocks.size() << "\n";

    // Write each block
    for (const auto &blk : blocks)
    {
        // Serialize the block
        std::string serializedBlock = blk.serialize();

        // Pack the serialized block into a buffer
        Buffer blockBuffer;
        blockBuffer.pack(serializedBlock);

        // Write the size of the packed block
        ofs << blockBuffer.getBuffer().size() << "\n";

        // Write the packed block data
        ofs << blockBuffer.getBuffer() << "\n";
    }

    ofs.close();
    return true;
}

/**
 * @brief Reads a blocked sequence set file.
 *
 * Reads blocks from a file, deserializing each block after unpacking. Expects the
 * file to have a header specifying the format and the number of blocks. Each
 * block is read using its size indicator and reconstructed.
 *
 * @param filename The name of the input file to read the blocks from.
 * @param blocks A vector to receive the deserialized blocks.
 * @return True if the operation succeeds, false otherwise.
 */
bool BlockBuffer::readBlocks(const std::string &filename, std::vector<Block> &blocks)
{
    std::ifstream ifs(filename, std::ios::in);
    if (!ifs)
    {
        std::cerr << "Error: Unable to open file for reading: " << filename << std::endl;
        return false;
    }

    std::string headerLine;
    std::getline(ifs, headerLine); // Read the header line

    // Read the number of blocks
    std::string blockCountLine;
    if (!std::getline(ifs, blockCountLine))
    {
        std::cerr << "Error: Failed to read block count from file" << std::endl;
        return false;
    }
    int blockCount = std::stoi(blockCountLine);

    // Read each block
    for (int i = 0; i < blockCount; i++)
    {
        // Read the length of the packed block
        std::string lengthLine;
        if (!std::getline(ifs, lengthLine))
        {
            std::cerr << "Error: Failed to read block length" << std::endl;
            return false;
        }
        int packedBlockSize = std::stoi(lengthLine);

        // Read the packed block data
        std::string packedBlock;
        packedBlock.resize(packedBlockSize);
        ifs.read(&packedBlock[0], packedBlockSize);

        // Unpack the block
        size_t commaPos = packedBlock.find(',');
        if (commaPos == std::string::npos)
        {
            std::cerr << "Error: Corrupted block data" << std::endl;
            return false;
        }
        int dataLength = std::stoi(packedBlock.substr(0, commaPos));
        std::string blockData = packedBlock.substr(commaPos + 1, dataLength);

        // Deserialize the block
        Block blk = Block::deserialize(blockData);
        blocks.push_back(blk);

        // Consume the trailing newline character
        ifs.ignore();
    }

    ifs.close();
    return true;
}