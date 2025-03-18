/** 
* Group 5 - Process zip codes from csv file
*/

// zip_code_buffer.cpp (Implementation File)
// This file contains the implementation of the ZipCodeBuffer and ZipCodeProcessor classes.
// It provides functionality for reading a CSV file, organizing zip code data by state,
// and identifying the easternmost, westernmost, northernmost, and southernmost zip codes for each state.

#include "zip_code_buffer.h"

/**
 * @brief Loads a CSV file containing zip code data into memory.
 * @param filename The name of the CSV file to read.
 * @return True if the file was loaded successfully, otherwise false.
 */
bool ZipCodeBuffer::loadCSV(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false;
    }

    std::string line;
    getline(file, line); // Skip the header line

    while (getline(file, line))
    {
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
 * @brief Retrieves all stored zip code records.
 * @return A vector of ZipCodeRecord objects.
 */
std::vector<ZipCodeRecord> ZipCodeBuffer::getRecords() const
{
    return records;
}

/**
 * @brief Organizes zip code records by state.
 * @param records A vector of ZipCodeRecord objects to be categorized by state.
 */
void ZipCodeProcessor::organizeByState(const std::vector<ZipCodeRecord> &records)
{
    for (const auto &record : records)
    {
        state_map[record.state].push_back(record);
    }
}

/**
 * @brief Finds the extreme zip codes (east, west, north, south) for each state.
 * @return A map where each state is mapped to its corresponding extreme zip codes.
 */
std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> ZipCodeProcessor::findExtremeZipCodes() const
{
    std::map<std::string, std::tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> extremes;

    for (const auto &[state, records] : state_map)
    {
        auto east = *std::min_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                      { return a.longitude < b.longitude; });
        auto west = *std::max_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                      { return a.longitude < b.longitude; });
        auto north = *std::max_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                       { return a.latitude < b.latitude; });
        auto south = *std::min_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                       { return a.latitude < b.latitude; });

        extremes[state] = std::make_tuple(east, west, north, south);
    }

    return extremes;
}

/**
 * @brief Prints the results in a formatted table.
 */
void ZipCodeProcessor::printResults() const
{
    std::cout << "State | Easternmost | Westernmost | Northernmost | Southernmost" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (const auto &[state, tuple] : findExtremeZipCodes())
    {
        const auto &[east, west, north, south] = tuple;
        std::cout << state << " | " << east.zip_code << " | " << west.zip_code << " | " << north.zip_code << " | " << south.zip_code << std::endl;
    }
}
