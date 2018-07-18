
#ifndef TELEMETRY_SYS_H_
#define TELEMETRY_SYS_H_

#include <string>

class TelemetrySys
{
public:
	TelemetrySys();
	void registerVar(std::string s, int va);
	void editVar(std::string name, int newVal);

	//set
	void setSymbol(std::string s);
	void setVal(void *v);

	//get
	std::string getSymbol();
	void *getVal();


private:
	 std::string symbol;
	 void *val;
};



#endif /* TELEMETRY_SYS_H_ */
