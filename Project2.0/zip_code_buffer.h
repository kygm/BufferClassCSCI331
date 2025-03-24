/**
    Group 5 - Process zip codes from csv file
**/
using namespace std;
#ifndef ZIP_CODE_BUFFER_H
#define ZIP_CODE_BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <limits>
#include <algorithm>
#include <unordered_map> // Added for primary key index functionality

/**
 * @struct ZipCodeRecord
 * @brief Stores information about a Zip Code.
 */
struct ZipCodeRecord
{
    string zip_code;
    string place_name;
    string state;
    string county;
    double latitude;
    double longitude;
};

/**
 * @class ZipCodeBuffer
 * @brief Reads and processes a CSV file containing Zip Code records.
 */
class ZipCodeBuffer
{
private:
    vector<ZipCodeRecord> records;

public:
    bool loadCSV(const string &filename);
    vector<ZipCodeRecord> getRecords() const;
};

/**
 * @class ZipCodeProcessor
 * @brief Processes and organizes Zip Code data.
 */
class ZipCodeProcessor
{
private:
    map<string, vector<ZipCodeRecord>> state_map;

public:
    void organizeByState(const vector<ZipCodeRecord> &records);
    map<string, tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> findExtremeZipCodes() const;
    void printResults() const;
};

// ==== Additional functions for Part II (Command-Line Search) ==== //

/**
 * @brief Builds a primary key index mapping each Zip Code to its file offset in the CSV file.
 * @param filename The name of the CSV file.
 * @return unordered_map<string, streampos> mapping from Zip Code to file offset.
 */
unordered_map<string, streampos> buildIndex(const string &filename);

/**
 * @brief Reads a single Zip Code record from the CSV file at a given file offset.
 *        Only the targeted record is loaded into RAM.
 * @param filename The name of the CSV file.
 * @param offset The file offset where the desired record begins.
 * @return ZipCodeRecord The parsed Zip Code record.
 */
ZipCodeRecord readRecordAtOffset(const string &filename, streampos offset);

/**
 * @brief Displays a ZipCodeRecord with each field labeled.
 * @param record The ZipCodeRecord to display.
 */
void displayRecord(const ZipCodeRecord &record);

#endif // ZIP_CODE_BUFFER_H
