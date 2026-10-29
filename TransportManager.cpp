#include "TransportManager.h"
#include "TransportExceptions.h"
#include<iostream>
#include <stdexcept>
#include<iomanip>
#include <fstream>
#include <sstream>
TransportManager::TransportManager() {
    networkName = "";
}
TransportManager::TransportManager(std::string name) {
    networkName = name;
}
int TransportManager::getStopsCount() const {
    return stops.size(); 
}
const BusStop& TransportManager::getStop(const std::string& code) const {
    auto it = stops.find(code);
    if (it == stops.end()) {
        throw MissingDataException("Stop " + code);
    }
    return it->second;
}

const TransitLine& TransportManager::getLine(const std::string& lineNum) const {
    auto it = lines.find(lineNum);
    if (it == lines.end()) {
        throw MissingDataException("Line " + lineNum);
    }
    return it->second;
}

void TransportManager::addStop(const BusStop& stop) {
    if (stop.getCode() == "" || stop.getName() == "") {
        throw TransportException("Error: Empty parameters!");
    }
    if (stops.find(stop.getCode()) != stops.end()) {
        throw DuplicateStopException(stop.getCode());
    }
    stops[stop.getCode()] = stop;
}

void TransportManager::addLine(const TransitLine& line) {
    if (line.getLine() == "") {
        throw TransportException("Error: Empty parameters!");
    }
    if (lines.find(line.getLine()) != lines.end()) {
        
        throw TransportException("Error: The line " + line.getLine() + " already exists!");
    }
    lines[line.getLine()] = line;
}
void TransportManager::addStopToLine(const std::string& lineNum, const std::string& stopCode) {
    if (lines.find(lineNum) == lines.end()) {
        throw MissingDataException("line " + lineNum);
    }
    if (stops.find(stopCode) == stops.end()) {
        throw MissingDataException("stop " + stopCode);
    }
    lines[lineNum].addStop(stops[stopCode]);
}
void TransportManager::printLinesForStop(const std::string& stopCode) const {
    if (stops.find(stopCode) == stops.end()) {
        throw MissingDataException("stop " + stopCode);
    }
    std::cout << "Bus Stop " << stopCode << " Lines: ";
    bool found = false;

    for (const auto& pair : lines) {
        const TransitLine& currentLine = pair.second;
        std::vector<BusStop> currentStops = currentLine.getStops();

        for (const auto& s : currentStops) {
            if (s.getCode() == stopCode) {
                std::cout << currentLine.getLine() << " ";
                found = true;
                break;
            }
        }
    }
    if (!found) {
        std::cout << "No lines.";
    }
    std::cout << "\n";
}
std::ostream& operator<<(std::ostream& os, const TransportManager::StopTimetableProxy& proxy) {
    auto stopIt = proxy.manager.stops.find(proxy.stopCode);
    if (stopIt == proxy.manager.stops.end()) {
        throw MissingDataException("Stop " + proxy.stopCode);
    }
    const BusStop& targetStop = stopIt->second;

    os << ">>> Electronic Board: Stop " << targetStop.getName()
        << " (" << proxy.stopCode << ") <<<\n";
    bool hasLines = false;
    for (const auto& pair : proxy.manager.lines) {
        const TransitLine& line = pair.second;
        const std::vector<BusStop>& currentStops = line.getStops();

        int stopIndex = -1;
        for (int i = 0; i < currentStops.size(); ++i) {
            if (currentStops[i].getCode() == proxy.stopCode) {
                stopIndex = i;
                break;
            }
        }

        if (stopIndex != -1) {
            hasLines = true;
            os << "--- Line " << line.getLine() << " ---\n";

            int travelTimeFromStart = stopIndex * 5;

            int currentH = line.getStartHour();
            int currentM = line.getStartMinute() + travelTimeFromStart;

            while (currentM >= 60) {
                currentH++;
                currentM -= 60;
                if (currentH >= 24) currentH -= 24;
            }

            int totalStartM = line.getStartHour() * 60 + line.getStartMinute();
            int totalEndM = line.getEndHour() * 60 + line.getEndMinute();

            int maxMinutes = 0;
            if (totalEndM >= totalStartM) {
                maxMinutes = totalEndM - totalStartM;
            }
            else {
                maxMinutes = (totalEndM + 1440) - totalStartM;
            }

            int elapsed = 0;
            int timesPrinted = 0;

            while (elapsed <= maxMinutes) {
                os << std::setfill('0') << std::setw(2) << currentH << ":"
                    << std::setfill('0') << std::setw(2) << currentM << "  ";
                timesPrinted++;
                if (timesPrinted % 10 == 0) {
                    os << "\n";
                }
                currentM += line.getInterval();
                elapsed += line.getInterval();
                while (currentM >= 60) {
                    currentH++;
                    currentM -= 60;
                    if (currentH >= 24) currentH -= 24;
                }
            }
            if (timesPrinted % 10 != 0) {
                os << "\n";
            }
            os << "\n";
        }
    }
    if (!hasLines) {
        os << "No lines pass through this stop!\n";
    }
    return os;
}

void TransportManager::saveToCSV(const std::string& stopsFilename, const std::string& linesFilename) const {
    std::ofstream stopsFile(stopsFilename);
    if (!stopsFile) throw TransportException("Error: Could not create stops file!");
    for (const auto& pair : stops) {
        stopsFile << pair.second.getCode() << ","
            << pair.second.getName() << ","
            << pair.second.getAddress() << ","
            << pair.second.getLatitude() << ","
            << pair.second.getLongitude() << "\n";
    }
    stopsFile.close();

    std::ofstream linesFile(linesFilename);
    if (!linesFile) throw TransportException("Error: Could not create lines file!");

    for (const auto& pair : lines) {
        const TransitLine& line = pair.second;
        linesFile << line.getLine() << "," << line.getType() << ","
            << line.getInterval() << "," << line.getStartHour() << ","
            << line.getStartMinute() << "," << line.getEndHour() << ","
            << line.getEndMinute();
        for (const auto& stop : line.getStops()) {
            linesFile << "," << stop.getCode();
        }
        linesFile << "\n";
    }
    linesFile.close();
}

void TransportManager::loadFromCSV(const std::string& stopsFilename, const std::string& linesFilename) {
    std::ifstream stopsFile(stopsFilename);
    if (stopsFile) {
        std::string lineStr;
        while (std::getline(stopsFile, lineStr)) {
            std::stringstream ss(lineStr);
            std::string code, name, address, latStr, lonStr;
            std::getline(ss, code, ',');
            std::getline(ss, name, ',');
            std::getline(ss, address, ',');
            std::getline(ss, latStr, ',');
            std::getline(ss, lonStr, ',');

            if (!code.empty()) {
                double lat = std::stod(latStr);
                double lon = std::stod(lonStr);
                addStop(BusStop(code, name, address, lat, lon));
            }
        }
    }
    stopsFile.close();

    std::ifstream linesFile(linesFilename);
    if (linesFile) {
        std::string lineStr;
        while (std::getline(linesFile, lineStr)) {
            std::stringstream ss(lineStr);
            std::string num, type, intervalStr, sH, sM, eH, eM, stopCode;
            std::getline(ss, num, ',');
            std::getline(ss, type, ',');
            std::getline(ss, intervalStr, ',');
            std::getline(ss, sH, ','); std::getline(ss, sM, ',');
            std::getline(ss, eH, ','); std::getline(ss, eM, ',');

            if (num.empty()) continue;
            TransitLine newLine(num, type, std::stoi(intervalStr),
                std::stoi(sH), std::stoi(sM),
                std::stoi(eH), std::stoi(eM));

            while (std::getline(ss, stopCode, ',')) {
                newLine.addStop(stops.at(stopCode));
            }
            addLine(newLine);
        }
        linesFile.close();
    }
}