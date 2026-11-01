#include "TransportManager.h"
#include "TransportExceptions.h"
#include<iostream>
#include <stdexcept>
#include<iomanip>
#include <fstream>
#include <sstream>
TransportManager::TransportManager() {
    networkName = "";
    db.initialize();
}
TransportManager::TransportManager(std::string name) {
    networkName = name;
    db.initialize();
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
    std::stringstream ss;
    ss << "INSERT INTO Stops (code, name, address, lat, lon) VALUES ('"
        << stop.getCode() << "', '" << stop.getName() << "', '"
        << stop.getAddress() << "', " << stop.getLatitude() << ", " << stop.getLongitude() << ");";

    db.executeQuery(ss.str());
}
void TransportManager::addLine(const TransitLine& line) {
    if (line.getLine() == "") {
        throw TransportException("Error: Empty parameters!");
    }
    if (lines.find(line.getLine()) != lines.end()) {
        throw TransportException("Error: A line with the number: " + line.getLine() + " already exists!");
    }

    lines[line.getLine()] = line;
    std::stringstream ss;
    ss << "INSERT INTO Lines (number, type, interval, start_h, start_m, end_h, end_m) VALUES ('"
        << line.getLine() << "', '" << line.getType() << "', "
        << line.getInterval() << ", " << line.getStartHour() << ", "
        << line.getStartMinute() << ", " << line.getEndHour() << ", "
        << line.getEndMinute() << ");";
    db.executeQuery(ss.str());

    const auto& route = line.getStops();
    for (size_t i = 0; i < route.size(); ++i) {
        std::stringstream rss;
        rss << "INSERT INTO RouteStops (line_number, stop_code, stop_order) VALUES ('"
            << line.getLine() << "', '" << route[i].getCode() << "', " << (i + 1) << ");";
        db.executeQuery(rss.str());
    }
}
void TransportManager::addStopToLine(const std::string& lineNum, const std::string& stopCode) {
    if (lineNum == "" || stopCode == "") {
        throw TransportException("Error: Empty parameters!");
    }
    if (lines.find(lineNum) == lines.end()) {
        throw MissingDataException("line " + lineNum);
    }
    if (stops.find(stopCode) == stops.end()) {
        throw MissingDataException("stop " + stopCode);
    }
    if (lines[lineNum].hasStop(stopCode)) {
        throw TransportException("Error: Stop " + stopCode + " is already in line " + lineNum);
    }
    lines[lineNum].addStop(stops[stopCode]);

    int newOrder = lines[lineNum].getStops().size();
    std::stringstream ss;
    ss << "INSERT INTO RouteStops (line_number, stop_code, stop_order) VALUES ('"
        << lineNum << "', '" << stopCode << "', " << newOrder << ");";
    db.executeQuery(ss.str());
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

void TransportManager::loadFromDatabase() {
    auto stopRows = db.selectQuery("SELECT * FROM Stops;");
    for (const auto& row : stopRows) {
        BusStop stop(
            row.at("code"),
            row.at("name"),
            row.at("address"),
            std::stod(row.at("lat")),
            std::stod(row.at("lon"))
        );
        stops[stop.getCode()] = stop;
    }

    auto lineRows = db.selectQuery("SELECT * FROM Lines;");
    for (const auto& row : lineRows) {
        TransitLine line(
            row.at("number"),
            row.at("type"),
            std::stoi(row.at("interval")),
            std::stoi(row.at("start_h")),
            std::stoi(row.at("start_m")),
            std::stoi(row.at("end_h")),
            std::stoi(row.at("end_m"))
        );
        lines[line.getLine()] = line;
    }

    auto routeRows = db.selectQuery("SELECT * FROM RouteStops ORDER BY line_number, stop_order ASC;");
    for (const auto& row : routeRows) {
        std::string lineNum = row.at("line_number");
        std::string stopCode = row.at("stop_code");
        if (lines.find(lineNum) != lines.end() && stops.find(stopCode) != stops.end()) {
            lines[lineNum].addStop(stops[stopCode]);
        }
    }
}
