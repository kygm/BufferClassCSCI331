#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <string>
#include <vector>
#include "Block.h"
#include "BlockBuffer.h"

/**
 * @brief Represents a B+ Tree implementation.
 *
 * Provides methods to construct, manage, and query a B+ Tree. This class handles
 * file-based storage for scalability, supporting dynamic insertion, deletion,
 * and search operations.
 */
class BPlusTree
{
public:
    /**
     * @brief Constructor for the BPlusTree class.
     *
     * Initializes the B+ Tree using the specified file and record-per-block settings.
     * Reads existing metadata from the file if available, or starts a new tree if not.
     *
     * @param filename The name of the file associated with the B+ Tree.
     * @param recordsPerBlock The maximum number of records allowed per block.
     */
    BPlusTree(const std::string &filename, int recordsPerBlock);

    /**
     * @brief Constructs the B+ Tree hierarchy from a set of leaf blocks.
     *
     * Writes all leaf blocks to the file and recursively creates index levels
     * until the root node is formed.
     *
     * @param blocks A vector of leaf blocks to form the base of the tree.
     */
    void buildTree(const std::vector<Block> &blocks);

    /**
     * @brief Dynamically inserts a new record into the B+ Tree.
     *
     * Finds the appropriate leaf block for the new record and inserts it. Handles
     * block overflows by splitting blocks and updating the tree structure as needed.
     *
     * @param record The record to be inserted into the tree.
     */
    void insert(const Record &record);

    /**
     * @brief Dynamically deletes a record from the B+ Tree.
     *
     * Removes the record with the specified key from the tree. Handles block
     * underflows by merging blocks and updating the tree structure.
     *
     * @param key The key of the record to delete.
     */
    void deleteRecord(int key);

    /**
     * @brief Searches for a record by key in the B+ Tree.
     *
     * Traverses the tree to locate the record with the specified key, starting
     * from the root block.
     *
     * @param key The key to search for.
     * @return The record associated with the key, or an empty record if not found.
     */
    Record search(int key);

    /**
     * @brief Dumps the structure of the B+ Tree for debugging purposes.
     *
     * Traverses the tree and prints the contents of each block, starting from
     * the root block.
     */
    void dumpTree();

private:
    // Metadata
    int rootRBN;          ///< Relative Block Number of the root block.
    int recordsPerBlock;  ///< Maximum number of records allowed per block.
    int totalBlocks;      ///< Total number of blocks currently in the tree.
    std::string filename; ///< Name of the file where the B+ Tree is stored.

    // File handler
    BlockBuffer buffer; ///< Manages file I/O operations for blocks.

    /**
     * @brief Recursively creates index levels for the B+ Tree.
     *
     * Aggregates keys and child block numbers into parent index blocks,
     * continuing until a single root node is formed.
     *
     * @param keys A vector of keys representing the largest keys from child blocks.
     * @param childRBNs A vector of Relative Block Numbers (RBNs) for the child blocks.
     * @return The RBN of the root index block created.
     */
    int createIndexLevel(const std::vector<int> &keys, const std::vector<int> &childRBNs);

    /**
     * @brief Splits a block on insertion.
     *
     * When a block overflows, its records are redistributed into two balanced
     * blocks. Handles updates to parent blocks or creates a new root block if needed.
     *
     * @param block The overflowing block to split.
     * @param parentRBN The RBN of the parent block, or -1 if the block has no parent.
     */
    void splitBlock(Block &block, int parentRBN);

    /**
     * @brief Merges two blocks on deletion.
     *
     * When a block underflows, its records are merged with a sibling block. Updates
     * the parent block to reflect the merged structure.
     *
     * @param leftBlock The left sibling block.
     * @param rightBlock The right sibling block.
     * @param parentRBN The RBN of the parent block.
     */
    void mergeBlocks(Block &leftBlock, Block &rightBlock, int parentRBN);
};

#endif