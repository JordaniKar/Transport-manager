#pragma once
#include<string>
class BusStop
{
private:
	std::string code;
	std::string name;
	std::string address;
	double latitude;
	double longitude;

public:
	BusStop();
	BusStop(std::string c, std::string n, std::string a, double lat, double lon);
	std::string getCode() const;
	std::string getName() const;
	std::string getAddress() const;
	double getLatitude() const;
	double getLongitude() const;

	void setCode(std::string c);
	void setName(std::string n);
	void setAddress(std::string a);
	void setLatitude(double lat);
	void setLongitude(double lon);
};

