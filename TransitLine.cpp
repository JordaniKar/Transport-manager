#include "TransitLine.h"
#include <iomanip>
#include"TransportExceptions.h"

TransitLine::TransitLine() {
	line = "";
	type = "";
	interval = 0;
    startHour = 0;
    startMinute = 0;
    endHour = 0;
    endMinute = 0;
}

TransitLine::TransitLine(std::string l, std::string t, int interv, int sHour, int sMin, int eHour, int eMin) {
    line = l;
    type = t;
    interval = interv;
    startHour = sHour;
    startMinute = sMin;
    endHour = eHour;
    endMinute = eMin;
}

std::string TransitLine::getLine() const {
	return line;
}
std::string TransitLine::getType() const {
	return type;
}
int TransitLine::getInterval() const {
	return interval;
}
std::vector<BusStop> TransitLine::getStops() const {
	return stops;
}
int TransitLine::getStartHour() const {
    return startHour;
}
int TransitLine::getStartMinute() const {
    return startMinute;
}
int TransitLine::getEndHour() const {
    return endHour; 
}
int TransitLine::getEndMinute() const {
    return endMinute; 
}
void TransitLine::setLine(std::string l) {
	line = l;
}
void TransitLine::setType(std::string t) {
	type = t;
}
void TransitLine::setInterval(int interv) {
	interval = interv;
}
void TransitLine::addStop(const BusStop& stop) {
	stops.push_back(stop);
}
void TransitLine::setStartHour(int hour) {
    if (hour < 0 || hour > 23) {
        throw TransportException("Error: Start hour must be between 0 and 23!");
    }
    startHour = hour;
}

void TransitLine::setStartMinute(int min) {
    if (min < 0 || min > 59) {
        throw TransportException("Error: Start minute must be between 0 and 59!");
    }
    startMinute = min;
}

void TransitLine::setEndHour(int hour) {
    if (hour < 0 || hour > 23) {
        throw TransportException("Error: End hour must be between 0 and 23!");
    }
    endHour = hour;
}

void TransitLine::setEndMinute(int min) {
    if (min < 0 || min > 59) {
        throw TransportException("Error: End minute must be between 0 and 59!");
    }
    endMinute = min;
}
std::ostream& operator<<(std::ostream& os, const TransitLine& line) {
    os << "=== Line " << line.getLine() << " (" << line.getType() << ") ===\n";
    os << "Route:\n";
    for (const auto& stop : line.getStops()) {
        os << " - " << stop.getName() << " (" << stop.getCode() << ")\n";
    }
    return os;
}

