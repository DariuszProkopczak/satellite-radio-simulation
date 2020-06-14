#include <cmath>

#include <iostream>

// taken from (and modified):
// https://github.com/overlord1123/LowPassFilter

class LowPassFilter{
public:
	LowPassFilter() {};

	LowPassFilter(double iCutOffFrequency, double iDeltaTime):
		output(0),
		ePow(1-exp(-iDeltaTime * 2 * M_PI * iCutOffFrequency)) {
	
	}

	void update_params(double iCutOffFrequency, double iDeltaTime) {
		output = 0;
		ePow = 1 - exp(-iDeltaTime * 2 * M_PI * iCutOffFrequency);
	}

	double update(double input) {
		return output += (input - output) * ePow;
	}
private:
	double output;
	double ePow;
};
