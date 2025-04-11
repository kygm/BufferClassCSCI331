#ifndef BLOCKBUFFER_H
#define BLOCKBUFFER_H

#include <string>
#include <vector>
#include "Block.h"

class BlockBuffer {
public:
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
};

#endif
