#include <orbit.hpp>
#include <rf.hpp>
#include <transceiver.hpp>
#include <cmath>
#include <tuple>
#include <cstdlib>

using namespace std;

class Satellite {
    // global container for satellite positions
    SatellitePositions *sat_positions;
    // transmitter and receiver both contain signal processing component,
    // and RF object
    unique_ptr<Transmitter> transmitter;
    unique_ptr<Receiver> receiver;
    double last_tx_processed_sample;    // last value that was processed by tx signal processor
    double last_received_rf_sample;     // last value that was recieved by antenna, befor being processed
    // velocity in m/s
    double vel_x;   
    double vel_y;
    double vel_z;
    double dt;      // time delta per time step in seconds
    int sat_id;

    // Selects a random position around the earth at the givern altitude, r.
    // Also selects a random velocity vector with magnitude required for
    // orbit.
    void set_random_pos_and_vel(double r) {

        double phi = (double) rand() / RAND_MAX * 2 * M_PI;
        double theta = (double) rand() / RAND_MAX * M_PI;

        // calculate position and set
        double x = r * sin(phi) * cos(theta);
        double y = r * sin(phi) * sin(theta);
        double z = r * cos(phi);
        this->sat_positions->set_position(this->sat_id, x, y, z);

        // calculate velocity vector and set
        tuple<double, double, double> vel = get_random_tangential_velocity(r, x, y, z);
        this->vel_x = get<0>(vel);
        this->vel_y = get<1>(vel);
        this->vel_z = get<2>(vel);
    }

public:

    Satellite(int sat_id_in, unique_ptr<AbstractSigProcFactory> &sig_proc_factory, SatellitePositions * sat_pos_in, EMField * em_field_in, double dt_in, double frequency, double r)
    {
        this->sat_id = sat_id_in;
        this->sat_positions = sat_pos_in;
        this->dt = dt_in;

        // randomly select position and velocity vectors
        set_random_pos_and_vel(r);

        this->transmitter = make_unique<Transmitter>(em_field_in, sat_id_in, sig_proc_factory, sat_pos_in, frequency, dt_in);
        this->receiver = make_unique<Receiver>(em_field_in, sat_id_in, sig_proc_factory, sat_pos_in, frequency, dt_in);
    }

    // TODO: implementation of exceptions
    // util::Expected<void> move_one_frame() {
    void move_one_frame() {
        tuple<double, double, double> gravity = calc_gravity(sat_positions->get_position(this->sat_id));
        this->vel_x += this->vel_x + (get<0>(gravity) * this->dt);
        this->vel_y += this->vel_y + (get<1>(gravity) * this->dt);
        this->vel_z += this->vel_z + (get<2>(gravity) * this->dt);

        // check orbit status and throw exception if invalid
        // tuple<double, double, double> x_y_z sat_positions->get_position(this->sat_id);
        // if sqrt(20000000 < get<0>(gravity)*get<0>(gravity) + get<1>(gravity)*get<1>(gravity) + get<2>(gravity)*get<2>(gravity))
        //     return std::range_error("Orbit Divergence Error");
        
        sat_positions->update_position(this->sat_id, this->vel_x * this->dt, this->vel_x * this->dt, this->vel_x * this->dt);
    }

    void retransmit() {
        double signal = this->receiver->receive_signal(0);
        this->last_received_rf_sample = this->receiver->get_last_received_rf_sample();
        this->transmitter->transmit_signal(signal, 0);
        this->last_tx_processed_sample = this->transmitter->get_last_processed_sample();
    }

    // Transmit value in "signal". To print debug info use
    // next method with "debug" argument.
    void transmit_signal(double signal)
    {
        this->transmitter->transmit_signal(signal, 0);
        this->last_tx_processed_sample = this->transmitter->get_last_processed_sample();
    }

    void transmit_signal(double signal, int debug)
    {
        this->transmitter->transmit_signal(signal, debug);
        this->last_tx_processed_sample = this->transmitter->get_last_processed_sample();
    }

    // Get received signal. To print debug info use
    // next method with "debug" argument.
    double receive_signal()
    {
        return this->receiver->receive_signal(0);
        this->last_received_rf_sample = this->receiver->get_last_received_rf_sample();
    }

    double receive_signal(int debug)
    {
        double signal = this->receiver->receive_signal(debug);
        this->last_received_rf_sample = this->receiver->get_last_received_rf_sample();
        return signal;
    }

    tuple<double, double, double> get_satellite_position() {

        tuple<double, double, double> pos_tuple = sat_positions->get_position(this->sat_id);

        double x_ret = get<0>(pos_tuple);
        double y_ret = get<1>(pos_tuple);
        double z_ret = get<2>(pos_tuple);

        double r = sqrt(x_ret * x_ret + y_ret * y_ret + z_ret * z_ret);
        double rho = atan(y_ret / x_ret);
        double theta = acos(z_ret / r);

        return tuple<double, double, double> {r, rho, theta};
    }

    double get_last_processed_tx_sample() { return this->last_tx_processed_sample; }
    double get_last_received_rf_sample() { return this->last_received_rf_sample; }
};