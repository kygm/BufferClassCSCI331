/**
 * @file main.cpp
 * @brief Converts CSV files to length-indicated format and creates primary key indices.
 *
 * This program processes CSV files to transform them into a length-indicated format.
 * It then creates an index based on the primary key (Zip Code) and writes this index to a file.
 * The program also supports searching for zip codes using the generated index.
 *
 * Usage:
 * Run the program with optional command-line arguments to search for specific zip codes.
 * Example: ./program -Z12345 -Z67890
 */

#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * @brief Converts a CSV file to a length-indicated format.
 *
 * Reads data from the input CSV file, packs each line into a length-indicated format,
 * and writes it to the output file.
 *
 * @param inputFile The name of the input CSV file.
 * @param outputFile The name of the output file to store length-indicated records.
 */
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

/**
 * @brief Creates an index mapping primary keys (Zip Codes) to file offsets.
 *
 * Reads the CSV file and extracts the Zip Code from each record. Stores the file
 * offset of each record in an unordered_map for fast lookup.
 *
 * @param filename The name of the file containing length-indicated records.
 * @return An unordered_map containing Zip Code keys and their respective file offsets.
 */
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

/**
 * @brief Writes the created index to a file.
 *
 * Outputs the Zip Code and corresponding file offset to a specified file.
 *
 * @param index The unordered_map containing Zip Code keys and file offsets.
 * @param filename The name of the output file where the index will be stored.
 */
void writeIndexToFile(const unordered_map<string, size_t> &index, const string &filename)
{
    ofstream outFile(filename);
    for (const auto &entry : index)
    {
        outFile << entry.first << "," << entry.second << endl;
    }
}

/**
 * @brief Main function to execute CSV conversion, indexing, and zip code lookup.
 *
 * Converts CSV files to length-indicated format, generates primary key indices,
 * and supports searching for zip codes using the index.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @return Exit status (0 on success, 1 on failure).
 */
int main(int argc, char *argv[])
{
    cout << "CSCI 331 Project 2.0\nConverting CSV files to length-indicated format and creating primary key indices...\n";

    // Convert CSV files to length-indicated format
    convertToLengthIndicated("resources/zipCodes.csv", "output/ordered_length_indicated.txt");
    convertToLengthIndicated("resources/randomized.csv", "output/randomized_length_indicated.txt");

    cout << "Creating and saving primary key indices...\n";

    // Create and save primary key indices
    unordered_map<string, size_t> orderedIndex = createIndex("output/ordered_length_indicated.txt");
    writeIndexToFile(orderedIndex, "output/ordered_index.txt");

    unordered_map<string, size_t> randomizedIndex = createIndex("output/randomized_length_indicated.txt");
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
            zipCodes.push_back(string(argv[i]).substr(2)); // Extract Zip Code
        }
    }

    cout << "\nSearching zipcodes in the indexed file..." << endl;

    // Search for Zip Codes
    for (const string &zipCode : zipCodes)
    {
        if (orderedIndex.find(zipCode) != orderedIndex.end())
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
