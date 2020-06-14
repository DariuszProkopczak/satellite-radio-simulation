#include <iostream>
// #include <expected/expected.h>
#include <satellite.hpp>
#include <data_source.hpp>
#include <versioning.hpp>
#include <IndentStream.hpp>

//
// External repos used:
//
// For signal processing:
// https://github.com/overlord1123/LowPassFilter
//
// For exceptions (implementation not complete):
// expected.h, written by Gilles Bellot, based on talk:
// "C++ and Beyond 2012: Andrei Alexandrescu - Systematic Error Handling in C++""
//
// IndendStream.hpp from Uchicago MPCS51045 Advanced C++
//

using namespace std;
using namespace version_1;

int main(int argc, char * argv[])
{
    int num_satellites = 2;
    unique_ptr<AbstractSigProcFactory> sig_proc_factory;
    vector<Satellite> satellites;
    tuple<double, double, double> position_holder;
    // vector<util::Expected<void>> exceptions(num_satellites);
    SatellitePositions sat_pos(num_satellites);
    EMField em_field(num_satellites);
    double frequency = 25000;
    double time_step = 1 / (frequency * 16);
    double num_time_steps = 10;
    int tx_satellite = 0;
    int rx_satellite = num_satellites - 1;
    double audio_signal;
    double audio_tone_frequency = 800;
    double gain = 10000;
    int print_signal = 1;
    int debug = 0;

    IndentStream ins(cout);
    ins << "Running Version #: " << version << endl;
    ins << "Version Name: " << version_msg << endl;

    // Parse Arguments and create correct factory for
    // singal processing type.
    if (argc == 1) {
        ins << "Please provide modulation method (AM or FM)" << endl;
        return 0;
    }
    else if (strcmp(argv[1], "AM") == 0) {
        sig_proc_factory = make_unique<AMProcessingFactory>();
    }
    else if (strcmp(argv[1], "FM") == 0) {
        sig_proc_factory = make_unique<FMProcessingFactory>();
    }
    else {
        ins << "Invalid Modulation method. Specify AM or FM." << endl;
        return 0;
    }

    // random values are used for satellite orbit initial conditions
    srand(7);

    // initialize satellites
    for (int i = 0; i < num_satellites; ++i)
        satellites.emplace_back(Satellite(i, sig_proc_factory, &sat_pos, &em_field, time_step, frequency, 8357000));

    // initialize tone generator
    WaveGenerator wave_gen(audio_tone_frequency, time_step, gain);

    // start simulation
    // loop once for each time step
    for (int i = 0; i < num_time_steps; ++i)
    {
        ins << "Time Step: " << i << indent << endl;

        // generates sample of sin wave
        audio_signal = wave_gen.get_next();
        ins << "Transmitted Audio Sample: " << audio_signal << endl;

        // move satellite one time step (Note that a bug occurs where satellite
        // positions rapidly diverge. Need to keep number of timesteps below 30.)
        satellites[tx_satellite].move_one_frame();
        position_holder = satellites[tx_satellite].get_satellite_position();
        ins << "Transmit Satellite Position: " << indent << endl;
        ins << "r: " << get<0>(position_holder)
            << " meters , rho: " << (180 / M_PI) * get<1>(position_holder)
            << " degrees, theta: " << (180 / M_PI) * get<2>(position_holder) 
            << " degrees " << unindent << endl;
        // exceptions[tx_satellite] = satellites[tx_satellite].move_one_frame();
        // transmit sin wave sample using transmission satellite
        satellites[tx_satellite].transmit_signal(audio_signal, debug);
        ins << "Transmitted RF Sample: " <<
                 satellites[tx_satellite].get_last_processed_tx_sample() << endl;

        // retransmit signal using non Tx/Rx satellites
        for (int j = 1; j < num_satellites-1; ++j)
        {
            // move satellite one time step
            satellites[j].move_one_frame();
            // exceptions[j] = satellites[j].move_one_frame();
            ins << "Satellite ID: "<< j <<  " Position: " << indent << endl;
            ins << "r: " << get<0>(position_holder)
                << " meters , rho: " << (180 / M_PI) * get<1>(position_holder)
                << " degrees, theta: " << (180 / M_PI) * get<2>(position_holder) 
                << " degrees " << unindent << endl;
            satellites[j].retransmit();
        }
        
        // move satellite one time step
        satellites[rx_satellite].move_one_frame();
        position_holder = satellites[rx_satellite].get_satellite_position();
        ins << "Receive Satellite Position: " << indent << endl;
        ins << "r: " << get<0>(position_holder)
            << " meters , rho: " << (180 / M_PI) * get<1>(position_holder)
            << " degrees, theta: " << (180 / M_PI) * get<2>(position_holder) 
            << " degrees " << unindent << endl;
        // exceptions[rx_satellite] = satellites[rx_satellite].move_one_frame();
        // receive signal 
        audio_signal = satellites[rx_satellite].receive_signal(debug);
        ins << "Received RF Sample: " <<
                 satellites[rx_satellite].get_last_received_rf_sample() << endl;

        // print signal
        ins << "Received Audio Sample: " << audio_signal << unindent << endl;

        // TODO: exception generation incomplete
        // for (int j = 0; j < num_satellites; ++j) {
        //     if (~exceptions[i].isValid()) {
        //         cout << "Exception Found! Satellited ID: " << i
        //              << "Time Step: " << j << endl;
        //         cout << "Exitting" << endl;
        //         return 0;
        //     }
        // }
    }

    return 0;
}
