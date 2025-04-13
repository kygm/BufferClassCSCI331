#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "BlockBuffer.h"
#include "Block.h"
#include "Record.h"

using namespace std;

/**
 * @brief Reads a CSV file (with a header) and returns a vector of CSV record strings.
 */
vector<string> readCSV(const string &filename) {
    vector<string> lines;
    ifstream ifs(filename);
    if (!ifs) {
        cerr << "Error opening CSV file: " << filename << endl;
        return lines;
    }
    string line;
    getline(ifs, line); // Skip header
    while (getline(ifs, line)) {
        if (!line.empty())
            lines.push_back(line);
    }
    return lines;
}

/**
 * @brief Creates blocks from CSV record strings.
 */
vector<Block> createBlocks(const vector<string> &records, int recordsPerBlock) {
    vector<Block> blocks;
    int blockNum = 0;
    for (size_t i = 0; i < records.size(); i += recordsPerBlock) {
        Block blk;
        blk.blockNumber = blockNum;
        int end = std::min((size_t)recordsPerBlock, records.size() - i);
        for (int j = 0; j < end; j++) {
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
 */
void dumpPhysical(const vector<Block>& blocks) {
    cout << "PHYSICAL BLOCK DUMP:\n";
    for (const auto &blk : blocks) {
        blk.dump();
        cout << "-------------------------" << endl;
    }
}

/**
 * @brief Dump blocks in logical order (following nextBlock pointer).
 */
void dumpLogical(const vector<Block>& blocks) {
    cout << "LOGICAL BLOCK DUMP:\n";
    int next = 0;
    while (next != -1 && next < blocks.size()) {
        blocks[next].dump();
        cout << "-------------------------" << endl;
        next = blocks[next].nextBlock;
    }
}

int main(int argc, char* argv[]) {
    string csvFilename = "./resources/zip_codes.csv";
    string blockFilename = "./output/blocked_sequence_set.txt";
    int recordsPerBlock = 3;

    bool dumpPhys = false;
    bool dumpLogic = false;

    // Parse command-line flags
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--dumpPhysical") dumpPhys = true;
        if (arg == "--dumpLogical") dumpLogic = true;
    }

    BlockBuffer bb;
    vector<Block> blocks;

    if (dumpPhys || dumpLogic) {
        // Read blocks from file
        if (!bb.readBlocks(blockFilename, blocks)) {
            cerr << "Error reading block file." << endl;
            return 1;
        }

        if (dumpPhys) dumpPhysical(blocks);
        if (dumpLogic) dumpLogical(blocks);
        return 0;
    }

    // GENERATION MODE: build blocks from CSV
    vector<string> csvRecords = readCSV(csvFilename);
    if (csvRecords.empty()) {
        cerr << "No records found in CSV file." << endl;
        return 1;
    }

    blocks = createBlocks(csvRecords, recordsPerBlock);

    if (!bb.writeBlocks(blockFilename, blocks)) {
        cerr << "Error writing block file." << endl;
        return 1;
    }

    cout << "Blocked sequence set file generated: " << blockFilename << endl;
    return 0;
}
