/** 
	Group 5 - Process zip codes from csv file
**/

// zip_code_buffer.h (Header File)

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

/**
 * @struct ZipCodeRecord
 * @brief Stores information about a Zip Code.
 */
struct ZipCodeRecord {
    std::string zip_code;
    std::string place_name;
    std::string state;
    std::string county;
    double latitude;
    double longitude;
};

/**
 * @class ZipCodeBuffer
 * @brief Reads and processes a CSV file containing Zip Code records.
 */
class ZipCodeBuffer {
private:
    std::vector<ZipCodeRecord> records;

public:
    bool loadCSV(const std::string& filename);
    std::vector<ZipCodeRecord> getRecords() const;
};

/**
 * @class ZipCodeProcessor
 * @brief Processes and organizes Zip Code data.
 */
class ZipCodeProcessor {
private:
    std::map<std::string, std::vector<ZipCodeRecord>> state_map;

public:
    void organizeByState(const std::vector<ZipCodeRecord>& records);
    std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> findExtremeZipCodes() const;
    void printResults() const;
};

#endif // ZIP_CODE_BUFFER_H
