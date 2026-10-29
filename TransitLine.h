#pragma once
#include <string>
#include <vector>
#include "BusStop.h"
class TransitLine
{
private:
	std::string line;
	std::string type;
	std::vector<BusStop> stops;
	int interval;
	int startHour;
	int startMinute;
	int endHour;
	int endMinute;

public:
	TransitLine();
	TransitLine(std::string l, std::string t, int interv, int sHour, int sMin, int eHour, int eMin);
	std::string getLine() const;
	std::string getType() const;
	int getInterval() const;
	std::vector<BusStop> getStops() const;
	int getStartHour() const;
	int getStartMinute() const;
	int getEndHour() const;
	int getEndMinute() const;

	void setLine(std::string l);
	void setType(std::string t);
	void setInterval(int interv);
	void addStop(const BusStop& stop);
	void setStartHour(int hour);
	void setStartMinute(int min);
	void setEndHour(int hour);
	void setEndMinute(int min);
	bool hasStop(const std::string& stopCode) const;
	friend std::ostream& operator<<(std::ostream& os, const TransitLine& line);
};


