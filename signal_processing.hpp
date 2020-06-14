#include <string_view>
#include <iostream>
#include <signal_processing_factory.hpp>
#include <LowPassFilter.hpp>


class SignalProcessing
{
    double carrier_frequency;    // Hz
    double dt = 0;               // seconds
    double time_since_start = 0; //seconds
public:
    SignalProcessing(double frequency_in, double dt_in) {
        this->carrier_frequency = frequency_in;
        this->dt = dt_in;
    }
    void set_dt(double dt_in) { this->dt = dt_in; }
    void set_frequency(double frequency_in) { this->carrier_frequency = frequency_in; }
    double get_dt() { return this->dt; }
    double get_frequency() { return this->carrier_frequency; }
    double get_time() { return this->time_since_start; }
    void increment_time() { this->time_since_start += dt; }
};

class TxProcessing;
class TxAMProcessing;
class TxFMProcessing;
class RxProcessing;
class RxAMProcessing;
class RxFMProcessing;

//
// visitors
//

struct TxProcessingVisitor {
    virtual void visit(TxProcessing &) const = 0;
    virtual void visit(TxAMProcessing &) const = 0;
    virtual void visit(TxFMProcessing &) const = 0;
};

struct RxProcessingVisitor {
    virtual void visit(RxProcessing &) const = 0;
    virtual void visit(RxAMProcessing &) const = 0;
    virtual void visit(RxFMProcessing &) const = 0;
};

//
// Base Classes
//

class TxProcessing : public SignalProcessing {
public:
    explicit TxProcessing(double frequency_in, double dt_in) : SignalProcessing(frequency_in, dt_in) { }
    virtual double process_tx_signal(double) = 0;
    virtual void set_parameters(double, double) = 0;
    virtual ~TxProcessing() = default;

    virtual void accept(TxProcessingVisitor const &v) { v.visit(*this); }
};

class RxProcessing : public SignalProcessing {
public:
    explicit RxProcessing(double frequency_in, double dt_in) : SignalProcessing(frequency_in, dt_in) { }
    virtual double process_rx_signal(double) = 0;
    virtual void set_parameters(double, double) = 0;
    virtual ~RxProcessing() = default;

    virtual void accept(RxProcessingVisitor const &v) { v.visit(*this); }
};

//
// AM Signal Processing Classes
//

class TxAMProcessing : public TxProcessing {
    double m;   // amplitude sensitivity
    double A;   // input signal amplitude

public:
    // set_parameters needs to be called after constructor due to factory implementation
    explicit TxAMProcessing() : TxProcessing(-1, -1) {
        this->m = 0.5;
        this->A = 1;
    }

    void set_parameters(double frequency_in, double dt_in) override {
        set_frequency(frequency_in);
        set_dt(dt_in);
    }

    double process_tx_signal(double signal) override {
        double tx_signal = ((signal / this->A) * this->m + 1) * sin(2 * M_PI * get_frequency() * get_time());
        increment_time();
        return tx_signal;
    }

    double get_m() { return this->m; }
    double get_A() { return this->A; }

    virtual void accept(TxProcessingVisitor const &v) override { v.visit(*this); }
};

class RxAMProcessing : public RxProcessing {
    LowPassFilter lpf;      // LowPassFilter class taken from:
                            // https://github.com/overlord1123/LowPassFilter

public:
    // set_parameters needs to be called after constructor due to factory implementation
    explicit RxAMProcessing() : RxProcessing(-1, -1) { }

    void set_parameters(double frequency_in, double dt_in) override {
        set_frequency(frequency_in);
        set_dt(dt_in);
        // low pass filter with bandwidth of 2*pi*10000 Hz
        lpf.update_params(10000, get_dt());
    }

    double process_rx_signal(double signal) override {
        // frequency shift
        double amplitude = sin(2 * M_PI * get_frequency() * get_time());
        double shifted_signal = signal * amplitude;

        increment_time();

        // low pass filter
        double filtered_signal = lpf.update(100 * shifted_signal);

        return filtered_signal;
    }

    virtual void accept(RxProcessingVisitor const &v) override { v.visit(*this); }
};
  
//
// FM Signal Processing Classes
//

class TxFMProcessing : public TxProcessing {

    double dev;   // frequency deviation

public:
    explicit TxFMProcessing() : TxProcessing(-1, -1) { }

    void set_parameters(double frequency_in, double dt_in) override {
        set_frequency(frequency_in);
        set_dt(dt_in);
        this->dev = 0.01 * frequency_in;   // set deviation to 1 % of carrier frequency
    }

    double process_tx_signal(double signal) override {
        // frequency shift
        double fm_signal = sin(2*M_PI*(get_frequency() + signal * this->dev) * get_time());

        increment_time();

        return fm_signal;
    }

    double get_dev() { return this->dev; }

    virtual void accept(TxProcessingVisitor const &v) override { v.visit(*this); }
};

class RxFMProcessing : public RxProcessing {
    double dev;   // frequency deviation
    LowPassFilter lpf_left;      
    LowPassFilter lpf_right;      

public:
    explicit RxFMProcessing() : RxProcessing(-1, -1) { }

    void set_parameters(double frequency_in, double dt_in) override {
        set_frequency(frequency_in);
        set_dt(dt_in);
        this->dev = 0.001 * frequency_in;   // set deviation to 1 % of carrier frequency
        // low pass filter with bandwidth of 2*pi*10000 Hz
        lpf_left.update_params(10000, get_dt());
        lpf_right.update_params(10000, get_dt());
    }

    double process_rx_signal(double signal) override {
        // Using method with two AM demodulators. One at each end of
        // the band. The difference of the am demodulators is calculated
        // to get the final signal.

        // frequency shift
        double am_shift_left = sin(2*M_PI*(get_frequency() + this->dev/2) * get_time());
        double am_shift_right = sin(2*M_PI*(get_frequency() - this->dev/2) * get_time());
        increment_time();

        // low pass filter
        double am_demod_left = lpf_left.update(100 * am_shift_left);
        double am_demod_right = lpf_right.update(100 * am_shift_right);

        double final_signal = am_demod_right - am_demod_left;

        return final_signal;
    }

    double get_dev() { return this->dev; }

    virtual void accept(RxProcessingVisitor const &v) override { v.visit(*this); }
};
