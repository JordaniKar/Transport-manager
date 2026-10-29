#pragma once
#include <stdexcept>
#include <string>
class TransportException : public std::runtime_error {
public:
    TransportException(const std::string& message) : std::runtime_error(message) {}
};
class DuplicateStopException : public TransportException {
public:
    DuplicateStopException(const std::string& stopCode)
        : TransportException("Error: A stop with the code: " + stopCode + " already exists!") {
    }
};

class MissingDataException : public TransportException {
public:
    MissingDataException(const std::string& item)
        : TransportException("Error: Missing " + item + " in the database!") {
    }
};
