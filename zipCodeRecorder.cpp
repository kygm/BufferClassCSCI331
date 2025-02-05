/*
	Group 5 - Process zip codes from csv file
*/

#ifndef ZIP_CODE_BUFFER_H
#define ZIP_CODE_BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <algorithm>

/**
 * @class ZipCodeRecord
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
    /**
     * @brief Reads a CSV file and stores records.
     * @param filename The path to the CSV file.
     */
    bool loadCSV(const std::string& filename);

    /**
     * @brief Returns the stored Zip Code records.
     * @return A vector of ZipCodeRecord objects.
     */
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
    /**
     * @brief Organizes records by state.
     * @param records A vector of ZipCodeRecord objects.
     */
    void organizeByState(const std::vector<ZipCodeRecord>& records);

    /**
     * @brief Finds extreme Zip Codes for each state.
     * @return A map containing state-wise extreme Zip Codes.
     */
    std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> findExtremeZipCodes() const;

    /**
     * @brief Prints the results in a table format.
     */
    void printResults() const;
};

#endif

