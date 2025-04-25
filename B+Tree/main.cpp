#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "BlockBuffer.h"
#include "Block.h"
#include "Record.h"
#include "BPlusTree.h"

using namespace std;

/**
 * @brief Reads a CSV file (with a header) and returns a vector of CSV record strings.
 *
 * Reads each line in the CSV file, skipping the header, and stores them in a vector.
 *
 * @param filename The name of the CSV file to read.
 * @return A vector of strings, each representing a record in the CSV file.
 */
vector<string> readCSV(const string &filename)
{
    vector<string> lines;
    ifstream ifs(filename);
    if (!ifs)
    {
        cerr << "Error opening CSV file: " << filename << endl;
        return lines;
    }
    string line;
    getline(ifs, line); // Skip header
    while (getline(ifs, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }
    return lines;
}

/**
 * @brief Creates blocks from CSV record strings.
 *
 * Splits the CSV records into fixed-sized blocks, with each block containing a
 * specified maximum number of records. Each block is sequentially numbered.
 *
 * @param records A vector of CSV record strings to split into blocks.
 * @param recordsPerBlock The maximum number of records allowed in each block.
 * @return A vector of Block objects containing the CSV records.
 */
vector<Block> createBlocks(const vector<string> &records, int recordsPerBlock)
{
    vector<Block> blocks;
    int blockNum = 0;
    for (size_t i = 0; i < records.size(); i += recordsPerBlock)
    {
        Block blk;
        blk.blockNumber = blockNum;
        int end = std::min((size_t)recordsPerBlock, records.size() - i);
        for (int j = 0; j < end; j++)
        {
            std::stringstream ss(records[i + j]);
            Record r;
            std::string token;
            getline(ss, token, ',');
            r.index = std::stoi(token);
            getline(ss, r.field1, ',');
            getline(ss, r.field2, ',');
            getline(ss, r.field3, ',');
            blk.records.push_back(r);
        }
        blk.nextBlock = (i + recordsPerBlock < records.size()) ? blockNum + 1 : -1;
        blocks.push_back(blk);
        blockNum++;
    }
    return blocks;
}

/**
 * @brief Dump blocks in physical order (as stored in file).
 *
 * Iterates through the provided blocks and prints their contents sequentially.
 *
 * @param blocks A vector of Block objects to dump.
 */
void dumpPhysical(const vector<Block> &blocks)
{
    cout << "PHYSICAL BLOCK DUMP:\n";
    for (const auto &blk : blocks)
    {
        blk.dump();
        cout << "-------------------------" << endl;
    }
}

/**
 * @brief Dump blocks in logical order (following nextBlock pointer).
 *
 * Traverses the blocks starting from the first block and follows the `nextBlock`
 * pointers to dump the logical structure of the blocks.
 *
 * @param blocks A vector of Block objects to dump.
 */
void dumpLogical(const vector<Block> &blocks)
{
    cout << "LOGICAL BLOCK DUMP:\n";
    int next = 0;
    while (next != -1 && next < blocks.size())
    {
        blocks[next].dump();
        cout << "-------------------------" << endl;
        next = blocks[next].nextBlock;
    }
}

/**
 * @brief Main entry point for the program.
 *
 * Parses command-line flags, reads records from a CSV file, and generates a
 * blocked sequence set and a B+ Tree. Also supports dumping the physical
 * and logical structure of blocks.
 *
 * Command-line flags:
 * - `--dumpPhysical`: Dumps the physical structure of the blocks.
 * - `--dumpLogical`: Dumps the logical structure of the blocks.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return An integer indicating the exit status of the program.
 */
int main(int argc, char *argv[])
{
    string csvFilename = "./resources/zip_codes.csv";           ///< Name of the CSV file to read.
    string blockFilename = "./output/blocked_sequence_set.txt"; ///< Name of the output file for blocks.
    int recordsPerBlock = 3;                                    ///< Maximum records per block.

    bool dumpPhys = false;  ///< Flag to indicate whether to dump physical structure.
    bool dumpLogic = false; ///< Flag to indicate whether to dump logical structure.

    // Parse command-line flags
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "--dumpPhysical")
            dumpPhys = true;
        if (arg == "--dumpLogical")
            dumpLogic = true;
    }

    BlockBuffer bb;       ///< BlockBuffer instance to handle file I/O operations.
    vector<Block> blocks; ///< Vector to store the blocks.

    if (dumpPhys || dumpLogic)
    {
        // Read blocks from file
        if (!bb.readBlocks(blockFilename, blocks))
        { // Pass the filename explicitly
            cerr << "Error reading block file: " << blockFilename << endl;
            return 1;
        }

        if (dumpPhys)
            dumpPhysical(blocks); // Physical structure of the blocks
        if (dumpLogic)
            dumpLogical(blocks); // Logical structure of the blocks
        return 0;
    }

    // GENERATION MODE: Read CSV and create blocks
    vector<string> csvRecords = readCSV(csvFilename); // Read CSV data
    if (csvRecords.empty())
    {
        cerr << "No records found in CSV file: " << csvFilename << endl;
        return 1;
    }

    blocks = createBlocks(csvRecords, recordsPerBlock); // Create blocks from CSV data

    // Write the generated blocks to the output file
    if (!bb.writeBlocks(blockFilename, blocks))
    { // Pass the filename explicitly
        cerr << "Error writing block file: " << blockFilename << endl;
        return 1;
    }

    // Create the BPlusTree from the blocks
    BPlusTree bTree(blockFilename, recordsPerBlock); // Initialize BPlusTree
    bTree.buildTree(blocks);                         // Use the generated blocks to build the tree

    // Dump the tree structure to verify correctness
    cout << "Dumping B+ Tree structure..." << endl;
    bTree.dumpTree();

    cout << "Blocked sequence set and B+ Tree file generated: " << blockFilename << endl;
    return 0;
}