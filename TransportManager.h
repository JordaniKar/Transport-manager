#pragma once
#include<string>
#include<unordered_map>
#include "BusStop.h"
#include "TransitLine.h"
class TransportManager
{
private:
	std::unordered_map<std::string, BusStop> stops;
	std::unordered_map<std::string, TransitLine> lines;
	std::string networkName;

public:
	struct StopTimetableProxy {
		const TransportManager& manager; 
		std::string stopCode;            
	};
	TransportManager();
	TransportManager(std::string name);
	int getStopsCount() const;
	const BusStop& getStop(const std::string& code) const;
	const TransitLine& getLine(const std::string& lineNum) const;
	void addStop(const BusStop& stop);
	void addLine(const TransitLine& line);

	void addStopToLine(const std::string& lineNum, const std::string& stopCode);
	void printLinesForStop(const std::string& stopCode) const;
	void printTimetableForStop(const std::string& stopCode) const;
	StopTimetableProxy getTimetable(const std::string& code) const {
		return { *this, code };
	}
	friend std::ostream& operator<<(std::ostream& os, const TransportManager::StopTimetableProxy& proxy);
	void saveToCSV(const std::string& stopsFilename, const std::string& linesFilename) const;
	void loadFromCSV(const std::string& stopsFilename, const std::string& linesFilename);
};

