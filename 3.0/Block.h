#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Record.h"
#include "Buffer.h"

class Block {
public:
    int blockNumber;           ///< Sequential number of the block.
    int nextBlock;             ///< Logical pointer to the next block (-1 if none).
    std::vector<Record> records; ///< List of records in this block.

    Block() : blockNumber(0), nextBlock(-1) {}

    /**
     * @brief Serializes the block to a string.
     *
     * First writes a header line: blockNumber,recordCount,nextBlock
     * Then, for each record, packs the record using Buffer and writes the result.
     *
     * @return The serialized block string.
     */
    std::string serialize() const {
        std::stringstream ss;
        // Write block header.
        ss << blockNumber << "," << records.size() << "," << nextBlock << "\n";
        // Write each record (packed with Buffer).
        for (const auto &rec : records) {
            Buffer buf;
            std::string recStr = rec.serialize();
            buf.pack(recStr);
            ss << buf.getBuffer() << "\n";
        }
        return ss.str();
    }

    /**
     * @brief Deserializes a block from a string.
     *
     * Expects the first line to be the block header.
     * Each subsequent line is a packed record.
     *
     * @param data The serialized block string.
     * @return A Block object.
     */
    static Block deserialize(const std::string &data) {
        Block blk;
        std::stringstream ss(data);
        std::string line;
        // Get header line.
        if (getline(ss, line)) {
            std::stringstream headerStream(line);
            std::string token;
            getline(headerStream, token, ',');
            blk.blockNumber = std::stoi(token);
            getline(headerStream, token, ','); // record count (not used here)
            int recordCount = std::stoi(token);
            getline(headerStream, token, ',');
            blk.nextBlock = std::stoi(token);
        }
        // Read each packed record.
        while (getline(ss, line)) {
            if (line.empty())
                continue;
            // Unpack the record manually.
            size_t commaPos = line.find(',');
            if (commaPos == std::string::npos)
                continue;
            int len = std::stoi(line.substr(0, commaPos));
            std::string recData = line.substr(commaPos + 1, len);
            Record r = Record::deserialize(recData);
            blk.records.push_back(r);
        }
        return blk;
    }

    /**
     * @brief Dumps the block content to standard output.
     */
    void dump() const {
        std::cout << "Block Number: " << blockNumber << ", Next Block: " << nextBlock << std::endl;
        std::cout << "Records:" << std::endl;
        for (const auto &r : records) {
            std::cout << r.index << " | " << r.field1 << " | " << r.field2 << " | " << r.field3 << std::endl;
        }
    }
};

#endif
