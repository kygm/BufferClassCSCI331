/**
 * Group 5 - Process zip codes from csv file
 */

// -----------------------------------------------------------------
// Part I: Existing functions for loading and processing CSV records
// -----------------------------------------------------------------

#include "zip_code_buffer.h"
using namespace std;

// Standard headers already included via zip_code_buffer.h

/**
 * @brief Loads a CSV file containing zip code data into memory.
 * @param filename The name of the CSV file to read.
 * @return True if the file was loaded successfully, otherwise false.
 */
bool ZipCodeBuffer::loadCSV(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Skip the header line

    while (getline(file, line))
    {
        stringstream ss(line);
        ZipCodeRecord record;

        getline(ss, record.zip_code, ',');
        getline(ss, record.place_name, ',');
        getline(ss, record.state, ',');
        getline(ss, record.county, ',');
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
vector<ZipCodeRecord> ZipCodeBuffer::getRecords() const
{
    return records;
}

/**
 * @brief Organizes zip code records by state.
 * @param records A vector of ZipCodeRecord objects to be categorized by state.
 */
void ZipCodeProcessor::organizeByState(const vector<ZipCodeRecord> &records)
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
map<string, tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> ZipCodeProcessor::findExtremeZipCodes() const
{
    map<string, tuple<ZipCodeRecord, ZipCodeRecord, ZipCodeRecord, ZipCodeRecord>> extremes;

    for (const auto &[state, records] : state_map)
    {
        auto east = *min_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                 { return a.longitude < b.longitude; });
        auto west = *max_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                 { return a.longitude < b.longitude; });
        auto north = *max_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                  { return a.latitude < b.latitude; });
        auto south = *min_element(records.begin(), records.end(), [](const ZipCodeRecord &a, const ZipCodeRecord &b)
                                  { return a.latitude < b.latitude; });

        extremes[state] = make_tuple(east, west, north, south);
    }

    return extremes;
}

/**
 * @brief Prints the results in a formatted table.
 */
void ZipCodeProcessor::printResults() const
{
    cout << "State | Easternmost | Westernmost | Northernmost | Southernmost" << endl;
    cout << "------------------------------------------------------------" << endl;

    for (const auto &[state, tuple] : findExtremeZipCodes())
    {
        const auto &[east, west, north, south] = tuple;
        cout << state << " | " << east.zip_code << " | " << west.zip_code << " | " << north.zip_code << " | " << south.zip_code << endl;
    }
}

// -----------------------------------------------------------------
// Part II: New functions for command-line search functionality
// -----------------------------------------------------------------

/**
 * @brief Builds a primary key index mapping each Zip Code to its file offset in the CSV file.
 * @param filename The CSV file name.
 * @return unordered_map<string, streampos> The built index.
 */
unordered_map<string, streampos> buildIndex(const string &filename)
{
    unordered_map<string, streampos> index;
    ifstream file(filename, ios::binary); // Open in binary mode
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    // Read and skip the header line
    string header;
    getline(file, header);

    while (file)
    {
        streampos pos = file.tellg(); // Record position at start of line
        string line;
        if (!getline(file, line))
            break;
        stringstream ss(line);
        string zip;
        getline(ss, zip, ',');

        if (!zip.empty())
            index[zip] = pos;
    }
    file.close();
    return index;
}

/**
 * @brief Reads a single Zip Code record from the CSV file at the given file offset.
 *        Only the targeted record is loaded into RAM.
 * @param filename The CSV file name.
 * @param offset The file offset where the record begins.
 * @return ZipCodeRecord The parsed record.
 */
ZipCodeRecord readRecordAtOffset(const string &filename, streampos offset)
{

    ZipCodeRecord record;

    ifstream file(filename, ios::binary);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    file.seekg(offset);
    string line;
    getline(file, line);

    stringstream ss(line);

    getline(ss, record.zip_code, ',');
    getline(ss, record.place_name, ',');
    getline(ss, record.state, ',');
    getline(ss, record.county, ',');
    string latStr, lonStr;
    getline(ss, latStr, ',');
    getline(ss, lonStr, ',');

    record.latitude = stod(latStr);

    record.longitude = stod(lonStr);

    file.close();
    return record;
}

/**
 * @brief Displays a ZipCodeRecord with each field labeled.
 * @param record The record to display.
 */
void displayRecord(const ZipCodeRecord &record)
{
    cout << "Zip Code: " << record.zip_code << "\n"
         << "Place Name: " << record.place_name << "\n"
         << "State: " << record.state << "\n"
         << "County: " << record.county << "\n"
         << "Latitude: " << record.latitude << "\n"
         << "Longitude: " << record.longitude << "\n";
}
