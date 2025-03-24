/**
 * @file main.cpp
 * @brief Part II: Command-Line Search for Zip Code Records.
 *
 * This program builds a primary key index from the CSV data file ("zip_codes.csv")
 * and processes command-line flags of the form -Z[ZipCode] to search for specific Zip Code records.
 * For each search, only the targeted record is loaded into memory.
 */

#include <iostream>
#include <cstring>
#include <unordered_map>
#include "zip_code_buffer.h"

using namespace std;

int main(int argc, char *argv[])
{

    string data_filename = "";

    int selection = 0;
    cout << "CSCI 331 Zip Code Project 2.0\nEnter 0 to use the randomized csv file, or anything else to use the nonrandomized csv file: ";
    cin >> selection;

    if (selection != 0)
        data_filename = "zip_codes.csv"; // CSV data file name
    else
        data_filename = "randomized.csv";

    // Build the primary key index from the CSV file.
    unordered_map<string, streampos> index = buildIndex(data_filename);

    // If no command-line arguments are provided, show usage and exit.
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " -Z[ZipCode] ..." << endl;
        return 0;
    }

    // Process each command-line argument that starts with "-Z"
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg.rfind("-Z", 0) == 0 && arg.size() > 2)
        {                                     // Check for "-Z" flag
            string searchZip = arg.substr(2); // Extract zip code from flag
            cout << "Searching for Zip Code: " << searchZip << endl;
            auto it = index.find(searchZip);
            if (it == index.end())
            {
                cout << "Zip Code " << searchZip << " not found." << endl;
            }
            else
            {
                // Load only the targeted record into RAM.
                ZipCodeRecord record = readRecordAtOffset(data_filename, it->second);
                displayRecord(record);
            }
        }
        cout << "\n";
    }

    return 0;
}
