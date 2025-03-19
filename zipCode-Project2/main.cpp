#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

// Function to convert CSV to length-indicated format
void convertToLengthIndicated(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile);
    Buffer buffer;

    // Write header
    std::string header;
    std::getline(inFile, header);
    buffer.pack(header);
    buffer.writeHeader(outFile);

    // Process records
    std::string line;
    while (std::getline(inFile, line)) {
        buffer.pack(line);
        outFile << buffer.unpack() << std::endl;
    }
}

// Function to create a primary key index
std::unordered_map<std::string, size_t> createIndex(const std::string& filename) {
    std::unordered_map<std::string, size_t> index;
    std::ifstream file(filename);
    size_t offset = 0;
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::string zipCode = line.substr(0, line.find(','));  // Extract Zip Code
        index[zipCode] = offset;
        offset = file.tellg();
    }

    return index;
}

// Function to write index to a file
void writeIndexToFile(const std::unordered_map<std::string, size_t>& index, const std::string& filename) {
    std::ofstream outFile(filename);
    for (const auto& entry : index) {
        outFile << entry.first << "," << entry.second << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Convert CSV files to length-indicated format
    convertToLengthIndicated("original.csv", "original_length_indicated.txt");
    convertToLengthIndicated("randomized.csv", "randomized_length_indicated.txt");

    // Create and save primary key indexes
    auto originalIndex = createIndex("original_length_indicated.txt");
    writeIndexToFile(originalIndex, "original_index.txt");

    auto randomizedIndex = createIndex("randomized_length_indicated.txt");
    writeIndexToFile(randomizedIndex, "randomized_index.txt");

    // Handle command-line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " -Z<zipCode1> -Z<zipCode2> ..." << std::endl;
        return 1;
    }

    std::vector<std::string> zipCodes;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]).substr(0, 2) == "-Z") {
            zipCodes.push_back(std::string(argv[i]).substr(2));  // Fix: Convert char* to std::string
        }
    }

    // Search for Zip Codes
    for (const auto& zipCode : zipCodes) {
        if (originalIndex.find(zipCode) != originalIndex.end()) {
            std::cout << "Found: " << zipCode << std::endl;
        } else {
            std::cout << "Not Found: " << zipCode << std::endl;
        }
    }

    return 0;
}