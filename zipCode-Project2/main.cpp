#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

// Function to convert CSV to length-indicated format
void convertToLengthIndicated(const string &inputFile, const string &outputFile)
{
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);
    Buffer buffer;

    // Write header
    string header;
    getline(inFile, header);
    buffer.pack(header);
    buffer.writeHeader(outFile);

    // Process records
    string line;
    while (getline(inFile, line))
    {
        buffer.pack(line);
        outFile << buffer.unpack() << endl;
    }
}

// Function to create a primary key index
unordered_map<string, size_t> createIndex(const string &filename)
{
    unordered_map<string, size_t> index;
    ifstream file(filename);
    size_t offset = 0;
    string line;

    // Skip header
    getline(file, line);

    while (getline(file, line))
    {
        string zipCode = line.substr(0, line.find(',')); // Extract Zip Code
        index[zipCode] = offset;
        offset = file.tellg();
    }

    return index;
}

// Function to write index to a file
void writeIndexToFile(const unordered_map<string, size_t> &index, const string &filename)
{
    ofstream outFile(filename);
    for (const auto &entry : index)
    {
        outFile << entry.first << "," << entry.second << endl;
    }
}

int main(int argc, char *argv[])
{
    // Convert CSV files to length-indicated format
    convertToLengthIndicated("resources/zipCodes.csv", "output/original_length_indicated.txt");
    convertToLengthIndicated("resources/randomized.csv", "output/randomized_length_indicated.txt");

    // Create and save primary key indexes
    auto originalIndex = createIndex("output/original_length_indicated.txt");
    writeIndexToFile(originalIndex, "output/original_index.txt");

    auto randomizedIndex = createIndex("output/randomized_length_indicated.txt");
    writeIndexToFile(randomizedIndex, "output/randomized_index.txt");

    // Handle command-line arguments
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " -Z<zipCode1> -Z<zipCode2> ..." << endl;
        return 1;
    }

    vector<string> zipCodes;
    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]).substr(0, 2) == "-Z")
        {
            zipCodes.push_back(string(argv[i]).substr(2)); // Fix: Convert char* to string
        }
    }

    // Search for Zip Codes
    for (const auto &zipCode : zipCodes)
    {
        if (originalIndex.find(zipCode) != originalIndex.end())
        {
            cout << "Found: " << zipCode << endl;
        }
        else
        {
            cout << "Not Found: " << zipCode << endl;
        }
    }

    return 0;
}