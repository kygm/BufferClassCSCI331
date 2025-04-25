#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <sstream>

class Record {
public:
    int index;
    std::string field1;
    std::string field2;
    std::string field3;

    Record() : index(0) {}

    /**
     * @brief Serializes the record as a CSV string.
     *
     * Format: index,field1,field2,field3
     */
    std::string serialize() const {
        std::stringstream ss;
        ss << index << "," << field1 << "," << field2 << "," << field3;
        return ss.str();
    }

    /**
     * @brief Deserializes a CSV string into a Record.
     *
     * @param data The CSV string.
     * @return A Record object.
     */
    static Record deserialize(const std::string &data) {
        Record r;
        std::stringstream ss(data);
        std::string token;
        getline(ss, token, ',');
        r.index = std::stoi(token);
        getline(ss, r.field1, ',');
        getline(ss, r.field2, ',');
        getline(ss, r.field3, ',');
        return r;
    }
};

#endif
