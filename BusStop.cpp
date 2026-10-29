#include "BusStop.h"
BusStop::BusStop() {
	code = "";
	name = "";
	address = "";
	latitude = 0.0;
	longitude = 0.0;
}
BusStop::BusStop(std::string c, std::string n, std::string a, double lat, double longt) {
	code = c;
	name = n;
	address = a;
	latitude = lat;
	longitude = longt;
}
std::string BusStop::getCode() const {
	return code;
}
std::string BusStop::getName() const {
	return name;
}
std::string BusStop::getAddress() const {
	return address;
}
double BusStop::getLatitude() const{
	return latitude;
}
double BusStop::getLongitude() const {
	return longitude;
}
void BusStop::setCode(std::string c) {
	code = c;
}
void BusStop::setName(std::string n) {
	name = n;
}
void BusStop::setAddress(std::string a) {
	address = a;
}
void BusStop::setLatitude(double lat) {
	latitude = lat;
}
void BusStop::setLongitude(double lon) {
	longitude = lon;
}
