#ifndef TELEMETRY_H
#define TELEMETRY_H
#include <string>
#include <map>
#include "stdio.h"

class TelemetrySys {

public:
 	void registerVar(std::string name);
	void *getValue(std::string name);
	void setValue(std::string name, void *value);
	void editVar(std::string name, void* newValue);

private:
	std::map<std::string, int> nameMap;
};

#endif
