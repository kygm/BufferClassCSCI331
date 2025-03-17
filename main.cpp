/** 
	Group 5 - Process zip codes from csv file
    
    @file main.cpp
    @author Kiran Kadariya, Sargar Neupane, Kevin G, Abdul 
    @date 02-17-2025
    
********/


#include <iostream>
#include "zip_code_buffer.h"

int main() {
    ZipCodeBuffer buffer;
    ZipCodeProcessor processor;

    // Load the CSV file
    if (!buffer.loadCSV("zip_codes.csv")) {
        std::cerr << "Failed to load CSV file!" << std::endl;
        return 1; // Exit with error code
    }

    // Get records and process them
    std::vector<ZipCodeRecord> records = buffer.getRecords();
    processor.organizeByState(records);
    
    // Print the results
    processor.printResults();

    return 0;
}
