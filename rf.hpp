#include <iostream>
#include <vector>
#include <em_field.hpp>
#include <rf_buffer.hpp>

using namespace std;

// calculate ratio of amplitude at end of
// path to amplitude at start of path
double propagation_loss(double distance) {
    if (distance <= 1)
        return 1;
    else
        return 1 / distance;
}


// RF class
// used to represent the value of an RF
// signal propagating through space
class RF {

    EMField *em_field;
    int sat_id;

public:
    RF(EMField * em_field_in, int sat_id) {
        this->em_field = em_field_in;
        this->sat_id = sat_id;
    }

    EMField *get_em_field() { return this->em_field; }
    int get_sat_id() { return this->sat_id; }
};

// abstract transmitting RF class
class RFTx : RF
{

    // Buffers the signal to be transmitted.
    // This is needed to due latency between transmission
    // and time that another satellite receives signal.
    // The buffer is represented using a reyclable vector.
    RecycledRFBuffer<double>* rf_buffer;
    double buffer_max_size;
    SatellitePositions *sat_pos;
    // time step size
    double dt;
    double c = 299792458;
    double time_steps_no_signal = 0;
    double max_time_steps_no_signal;
    double sig_thresh = 1e-20; // if no signal above this for some amount of time, delete buffer

    double calc_field_at_satellite(SatellitePositions *sat_pos, int rx_sat_id) {
        // update electric field of other satellite base on current satellite's 
        // previous transmissions

        double distance = sat_pos->calc_distance(get_sat_id(), rx_sat_id);
        double time_to_rx_sat = distance / get_c();
        int sig_buff_size = rf_buffer->size();

        int time_steps_to_rx_sat = (int) round(time_to_rx_sat / get_dt());

        if (time_steps_to_rx_sat > sig_buff_size)
            // signal hasn't reached satellite
            return 0;

        // resize buffer once it gets too large
        if (sig_buff_size > this->buffer_max_size)
            rf_buffer->resize(round(0.85 * sig_buff_size));

        // get value of electric field and calculate loss
        double signal_at_rx_raw = (*rf_buffer)[sig_buff_size - time_steps_to_rx_sat - 1];
        signal_at_rx_raw = signal_at_rx_raw * propagation_loss(distance);

        return signal_at_rx_raw;

    }

    void check_buffer_activity(double in_signal) {
        // Checks if rf buffer has been updated with
        // any signal above a threshold recently. If not,
        // buffer is freed to save space. This way only
        // active transmitters have allocated memory.
        if (in_signal < this->sig_thresh)
        {
            this->time_steps_no_signal++;
            if (this->time_steps_no_signal >= this->max_time_steps_no_signal)
            {
                if (rf_buffer != NULL)
                {
                    delete rf_buffer;
                    rf_buffer = NULL;
                }
            }
            return;
        }
        else 
        {
            time_steps_no_signal = 0;
            if (rf_buffer == NULL)
                rf_buffer = new RecycledRFBuffer<double>();
        }
    }

public:
    explicit RFTx(EMField * em_field_in, int sat_id, SatellitePositions * sat_pos, double dt_in) : RF(em_field_in, sat_id) {
        this->sat_pos = sat_pos;
        this->dt = dt_in;
        // Create RF buffer.
        // Assume furthest two satellites can be is 
        // <earth diameter> + 2*<LEO altidue> + 4e6 ~= 12e6 + 2*2e6 + 4e6
        // Then calculate number of maximum necessary timesteps 
        // between two satellites.
        this->buffer_max_size = 20000000 / (this->c * this->dt);
        this->max_time_steps_no_signal = this->buffer_max_size;
        rf_buffer = new RecycledRFBuffer<double>();
    }

    // Takes input signal and updates RF signal at 
    // neighboring satellites to a value that 
    // represents the transmitted signal.
    void update_field(double in_signal) {

        if (rf_buffer != NULL)
            rf_buffer->push_back(in_signal);

        // free buffer is no signal received in a while
        check_buffer_activity(in_signal);

        // update the field of every other satellite
        SatellitePositions *sat_pos = get_sat_pos();
        for (int rx_sat_id = 0; rx_sat_id < sat_pos->get_num_sats(); ++rx_sat_id)
        {
            // satellite won't receive its own signal
            if (rx_sat_id == get_sat_id())
                continue;
            get_em_field()->set_field(rx_sat_id, get_sat_id(), calc_field_at_satellite(sat_pos, rx_sat_id));
        }
    }
    SatellitePositions *get_sat_pos() { return this->sat_pos; }
    double get_c() { return this->c; };
    double get_dt() { return this->dt; };

    ~RFTx() { delete rf_buffer; }
};

// abstract receiving RF class
// used to get the value of RF at
// a given satellite's receiver
class RFRx : RF
{
public:
    explicit RFRx(EMField * em_field_in, int sat_id) : RF(em_field_in, sat_id) {}
    double get_field() {
        double field = get_em_field()->get_field(get_sat_id());
        return field;
    }
};
