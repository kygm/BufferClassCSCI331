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
    bool loadCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return false;
        }
        std::string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            std::stringstream ss(line);
            ZipCodeRecord record;
            std::getline(ss, record.zip_code, ',');
            std::getline(ss, record.place_name, ',');
            std::getline(ss, record.state, ',');
            std::getline(ss, record.county, ',');
            ss >> record.latitude;
            ss.ignore();
            ss >> record.longitude;
            records.push_back(record);
        }
        file.close();
        return true;
    }

    /**
     * @brief Returns the stored Zip Code records.
     * @return A vector of ZipCodeRecord objects.
     */
    std::vector<ZipCodeRecord> getRecords() const {
        return records;
    }
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
    void organizeByState(const std::vector<ZipCodeRecord>& records) {
        for (const auto& record : records) {
            state_map[record.state].push_back(record);
        }
    }

    /**
     * @brief Finds extreme Zip Codes for each state.
     * @return A map containing state-wise extreme Zip Codes.
     */
    std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> findExtremeZipCodes() const {
        std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> extremes;
        for (const auto& [state, records] : state_map) {
            auto east = *std::min_element(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) { return a.longitude < b.longitude; });
            auto west = *std::max_element(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) { return a.longitude < b.longitude; });
            auto north = *std::max_element(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) { return a.latitude < b.latitude; });
            auto south = *std::min_element(records.begin(), records.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) { return a.latitude < b.latitude; });
            extremes[state] = std::make_tuple(east, west, north, south);
        }
        return extremes;
    }

    /**
     * @brief Prints the results in a table format.
     */
    void printResults() const {
        std::cout << "State | Easternmost | Westernmost | Northernmost | Southernmost" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        for (const auto& [state, tuple] : findExtremeZipCodes()) {
            const auto& [east, west, north, south] = tuple;
            std::cout << state << " | " << east.zip_code << " | " << west.zip_code << " | " << north.zip_code << " | " << south.zip_code << std::endl;
        }
    }
};

#endif

