#include "BPlusTree.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::upper_bound

#include "BPlusTree.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::upper_bound

/**
 * @brief Constructor for the BPlusTree class.
 *
 * Initializes the B+ Tree from a file or starts a new tree if the file does not exist.
 * The constructor attempts to read the file header to retrieve metadata such as
 * `recordsPerBlock`, `totalBlocks`, and `rootRBN`.
 *
 * @param filename The name of the file associated with the B+ Tree.
 * @param recordsPerBlock The maximum number of records allowed per block.
 */
BPlusTree::BPlusTree(const std::string &filename, int recordsPerBlock)
    : filename(filename), recordsPerBlock(recordsPerBlock), buffer(filename), rootRBN(-1), totalBlocks(0)
{
    // Attempt to read the header from the file
    std::ifstream file(filename);
    if (file)
    {
        std::string header;
        std::getline(file, header); // Read the header
        std::istringstream iss(header);
        std::string token;

        // Parse the header
        std::getline(iss, token, ','); // "BSS"
        if (token != "BSS")
        {
            throw std::runtime_error("Invalid file format");
        }
        std::getline(iss, token, ','); // Version (e.g., "1.0")
        std::getline(iss, token, ','); // recordsPerBlock
        recordsPerBlock = std::stoi(token);
        std::getline(iss, token, ','); // totalBlocks
        totalBlocks = std::stoi(token);
        std::getline(iss, token, ','); // rootRBN
        rootRBN = std::stoi(token);
    }
}

/**
 * @brief Splits an overflowing block into two balanced blocks.
 *
 * When a block overflows, its records are redistributed into a new block, maintaining
 * the balance of the B+ Tree. If there is no parent block, a new root is created to
 * reference the split blocks.
 *
 * @param block The overflowing block that needs to be split.
 * @param parentRBN The Relative Block Number (RBN) of the parent block, or -1 if no parent exists.
 */
void BPlusTree::splitBlock(Block &block, int parentRBN)
{
    // Create a new block for the split
    Block newBlock;
    newBlock.blockType = block.blockType; // Same type as the current block (LEAF or INDEX)
    newBlock.blockNumber = totalBlocks++; // Assign a new block number

    // Determine the split point
    size_t splitPoint = block.records.size() / 2;

    // Move the second half of the records to the new block
    newBlock.records.assign(block.records.begin() + splitPoint, block.records.end());
    block.records.erase(block.records.begin() + splitPoint, block.records.end());

    // Update the nextBlock pointers if it's a leaf block
    if (block.blockType == BlockType::LEAF)
    {
        newBlock.nextBlock = block.nextBlock;
        block.nextBlock = newBlock.blockNumber;
    }

    // Write both blocks back to the file
    buffer.writeBlocks(filename, {block});
    buffer.writeBlocks(filename, {newBlock});

    // Update the parent block
    if (parentRBN == -1)
    {
        // No parent block, create a new root
        Block newRoot;
        newRoot.blockType = BlockType::INDEX;
        newRoot.blockNumber = totalBlocks++;

        // Add references to the two split blocks
        Record leftEntry, rightEntry;
        leftEntry.index = block.records.back().index;         // Largest key in the left block
        leftEntry.field1 = std::to_string(block.blockNumber); // Block number of the left block
        newRoot.records.push_back(leftEntry);

        rightEntry.index = newBlock.records.back().index;         // Largest key in the new (right) block
        rightEntry.field1 = std::to_string(newBlock.blockNumber); // Block number of the new block
        newRoot.records.push_back(rightEntry);

        // Write the new root to the file
        buffer.writeBlocks(filename, {newRoot});

        // Update the root RBN
        rootRBN = newRoot.blockNumber;
    }
    else
    {
        // Update the existing parent block
        std::vector<Block> blocks;
        buffer.readBlocks(filename, blocks); // Read the parent block
        Block parentBlock = blocks[parentRBN];

        Record newEntry;
        newEntry.index = newBlock.records.back().index;         // Largest key in the new block
        newEntry.field1 = std::to_string(newBlock.blockNumber); // Block number of the new block
        parentBlock.records.push_back(newEntry);

        // Sort the parent block's records
        std::sort(parentBlock.records.begin(), parentBlock.records.end(), [](const Record &a, const Record &b)
                  { return a.index < b.index; });

        // Check for overflow in the parent block and recursively split if needed
        if (parentBlock.records.size() > recordsPerBlock)
        {
            splitBlock(parentBlock, -1); // Pass the grandparent's RBN (-1 if root needs to be replaced)
        }

        // Write the updated parent block
        buffer.writeBlocks(filename, {parentBlock});
    }
}
// Build the tree from leaf blocks
/**
 * @brief Builds the B+ Tree from a set of leaf blocks.
 *
 * Writes all leaf blocks to the file and constructs the hierarchical index levels
 * to organize the tree. The root block number is updated after constructing the
 * index levels.
 *
 * @param blocks A vector of leaf blocks to form the base of the B+ Tree.
 */
void BPlusTree::buildTree(const std::vector<Block> &blocks)
{
    // Write all leaf blocks to the file
    for (const auto &block : blocks)
    {
        buffer.writeBlocks(filename, {block});
        totalBlocks++;
    }

    // Extract keys and RBNs for the leaf blocks
    std::vector<int> keys;
    std::vector<int> rbns;
    for (const auto &block : blocks)
    {
        keys.push_back(block.records.back().index); // Use the largest key in each block
        rbns.push_back(block.blockNumber);
    }

    // Construct the index levels recursively
    rootRBN = createIndexLevel(keys, rbns);
}

// Create index levels recursively
/**
 * @brief Recursively creates hierarchical index levels for the B+ Tree.
 *
 * Aggregates keys and block numbers from child blocks into parent index blocks.
 * Continues building levels until a single root index block is formed.
 *
 * @param keys A vector of keys representing the largest keys from child blocks.
 * @param childRBNs A vector of Relative Block Numbers (RBNs) for the child blocks.
 * @return The RBN of the root index block created.
 */
int BPlusTree::createIndexLevel(const std::vector<int> &keys, const std::vector<int> &childRBNs)
{
    const int maxEntriesPerIndex = recordsPerBlock - 1; // Reserve space for splitting
    std::vector<int> parentKeys;
    std::vector<int> parentRBNs;

    for (size_t i = 0; i < childRBNs.size(); i += maxEntriesPerIndex)
    {
        Block indexBlock;
        indexBlock.blockType = BlockType::INDEX;
        indexBlock.blockNumber = totalBlocks++;

        int end = std::min(i + maxEntriesPerIndex, childRBNs.size());
        for (size_t j = i; j < end; ++j)
        {
            Record record;
            record.index = keys[j];
            record.field1 = std::to_string(childRBNs[j]); // Store RBN in field1
            indexBlock.records.push_back(record);
        }

        buffer.writeBlocks(filename, {indexBlock});

        // Store the largest key and block number for the parent level
        parentKeys.push_back(indexBlock.records.back().index);
        parentRBNs.push_back(indexBlock.blockNumber);
    }

    // If there's only one index block at this level, it's the root
    if (parentKeys.size() == 1)
    {
        return parentRBNs[0];
    }

    // Otherwise, recursively build the next level up
    return createIndexLevel(parentKeys, parentRBNs);
}

// Insert a record dynamically
/**
 * @brief Inserts a new record into the B+ Tree.
 *
 * Finds the appropriate leaf block for the new record and inserts it. Handles
 * overflow by splitting blocks as necessary and updating the parent blocks.
 *
 * @param record The record to be inserted into the tree.
 */
void BPlusTree::insert(const Record &record)
{
    if (rootRBN == -1)
    {
        // Tree is empty, create the first leaf block
        Block leafBlock;
        leafBlock.blockType = BlockType::LEAF;
        leafBlock.blockNumber = totalBlocks++;
        leafBlock.records.push_back(record);

        buffer.writeBlocks(filename, {leafBlock});
        rootRBN = leafBlock.blockNumber; // Set the root
        return;
    }

    int currentRBN = rootRBN;
    Block currentBlock;

    while (true)
    {
        // Read the block
        std::vector<Block> blocks;
        buffer.readBlocks(filename, blocks);
        currentBlock = blocks[currentRBN];

        if (currentBlock.blockType == BlockType::LEAF)
        {
            // Insert record in sorted order
            currentBlock.records.push_back(record);
            std::sort(currentBlock.records.begin(), currentBlock.records.end(), [](const Record &a, const Record &b)
                      { return a.index < b.index; });

            // Check for overflow
            if (currentBlock.records.size() > recordsPerBlock)
            {
                splitBlock(currentBlock, -1); // Split block (passing no parent for now)
            }

            buffer.writeBlocks(filename, {currentBlock});
            return;
        }
        else
        {
            // Navigate down the index block
            bool found = false;
            for (const auto &rec : currentBlock.records)
            {
                if (record.index <= rec.index)
                {
                    currentRBN = std::stoi(rec.field1); // Navigate to the child RBN
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                currentRBN = std::stoi(currentBlock.records.back().field1); // Default to the last child
            }
        }
    }
}

// Dump the tree structure
/**
 * @brief Dumps the structure of the B+ Tree for debugging purposes.
 *
 * Traverses the tree and prints the contents of each block, starting from the root
 * block. Displays both leaf and index blocks.
 */
void BPlusTree::dumpTree()
{
    if (rootRBN == -1)
    {
        std::cout << "Tree is empty!" << std::endl;
        return;
    }

    std::cout << "Dumping B+ Tree Structure:" << std::endl;

    std::vector<int> toVisit = {rootRBN};
    while (!toVisit.empty())
    {
        int currentRBN = toVisit.back();
        toVisit.pop_back();

        // Read the block
        std::vector<Block> blocks;
        buffer.readBlocks(filename, blocks);
        Block block = blocks[currentRBN];

        // Print the block
        block.dump();

        if (block.blockType == BlockType::INDEX)
        {
            // Add child blocks to the visit list
            for (const auto &rec : block.records)
            {
                toVisit.push_back(std::stoi(rec.field1)); // RBN stored in field1
            }
        }
    }
}
