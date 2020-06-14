#include <iostream>
#include <cmath>
#include <tuple>

class WaveGenerator {
    // generates a sample of a sin wave per time step

    double dt;
    double frequency;
    double curr_time = 0;
    double gain;

public:
    WaveGenerator(double frequency_in, double dt_in, double gain_in) {
        this->dt = dt_in;
        this->frequency = frequency_in;
        this->gain = gain_in;
    }

    double get_next() {
        double signal = this->gain * sin(2 * M_PI * this->frequency * this->curr_time);
        this->curr_time += this->dt;
        return signal;
    }
};
