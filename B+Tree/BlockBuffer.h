#ifndef BLOCKBUFFER_H
#define BLOCKBUFFER_H

#include <string>
#include <vector>
#include "Block.h"

class BlockBuffer
{
private:
    std::string filename; // Store the file name for file operations

public:
    /**
     * @brief Default constructor.
     */
    BlockBuffer() = default;

    /**
     * @brief Constructor that accepts a filename for file operations.
     *
     * @param file The file name to initialize the BlockBuffer with.
     */
    explicit BlockBuffer(const std::string &file) : filename(file) {}

    /**
     * @brief Writes a blocked sequence set file.
     *
     * The file consists of:
     * - A file header (packed using Buffer)
     * - A line with the number of blocks
     * - For each block: a length indicator (the size of the packed block) and the packed block data.
     *
     * @param filename The output file name.
     * @param blocks A vector of blocks to write.
     * @return true on success.
     */
    bool writeBlocks(const std::string &filename, const std::vector<Block> &blocks);

    /**
     * @brief Reads a blocked sequence set file.
     *
     * @param filename The input file name.
     * @param blocks A vector to receive the blocks.
     * @return true on success.
     */
    bool readBlocks(const std::string &filename, std::vector<Block> &blocks);

    /**
     * @brief Accessor for the filename used in BlockBuffer.
     *
     * @return The file name associated with this BlockBuffer.
     */
    std::string getFilename() const { return filename; }

    /**
     * @brief Mutator to set the filename for the BlockBuffer.
     *
     * @param file The new file name to set.
     */
    void setFilename(const std::string &file) { filename = file; }
};

#endif