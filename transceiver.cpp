#include <string_view>
#include <iostream>
#include "signal_processing.cpp"
#include "signal_processing_visitor.cpp"

// initialize factory types
using AbstractSigProcFactory = signal_processing_factory<TxProcessing, RxProcessing>;
using AMProcessingFactory
= concrete_signal_processing_factory<AbstractSigProcFactory, TxAMProcessing, RxAMProcessing>;
using FMProcessingFactory
= concrete_signal_processing_factory<AbstractSigProcFactory, TxFMProcessing, RxFMProcessing>;

// Transmitter class. Each satellite has one. Contains a transmit signal processor
// and a transmit RF object.
class Transmitter {
    // Abstract tx signal processor. Subclasses can be tx AM, tx FM, etc.
    // Type is determined by the factory that is passed into Transmitter
    // constructor.
    unique_ptr<TxProcessing> tx_signal_processor;
    unique_ptr<RFTx> tx_rf;
    double last_processed_sample;

public:
    Transmitter(EMField * em_field_in, int sat_id, unique_ptr<AbstractSigProcFactory> &sig_proc_factory, SatellitePositions * sat_pos, double frequency_in, double dt_in) {
        // factory determines type of processor (AM, FM, etc.)
        this->tx_signal_processor = sig_proc_factory->create<TxProcessing>();
        this->tx_signal_processor->set_parameters(frequency_in, dt_in);

        // create tx rf object
        this->tx_rf = make_unique<RFTx>(em_field_in, sat_id, sat_pos, dt_in);
    }

    void transmit_signal(double signal, int print_status) {
        if (print_status)
            this->tx_signal_processor->accept(PrintTxProcParams());
        this->last_processed_sample = this->tx_signal_processor->process_tx_signal(signal);
        this->tx_rf->update_field(this->last_processed_sample);
    }

    double get_last_processed_sample() {
        return this->last_processed_sample;
    }
};

// Receiver class. Each satellite has one. Contains a receive signal processor
// and a receive RF object.
class Receiver {
    // Abstract rx signal processor. Subclasses can be rx AM, rx FM, etc.
    // Type is determined by the factory that is passed into Transmitter
    // constructor.
    unique_ptr<RxProcessing> rx_signal_processor;
    unique_ptr<RFRx> rx_rf;
    double last_received_rf_sample;

public:
    Receiver(EMField * em_field_in, int sat_id, unique_ptr<AbstractSigProcFactory> &sig_proc_factory, SatellitePositions * sat_pos, double frequency_in, double dt_in) {
        // factory determines type of processor (AM, FM, etc.)
        this->rx_signal_processor = sig_proc_factory->create<RxProcessing>();
        this->rx_signal_processor->set_parameters(frequency_in, dt_in);

        // create rx rf object
        this->rx_rf = make_unique<RFRx>(em_field_in, sat_id);
    }

    double receive_signal(int print_status) {
        if (print_status)
            this->rx_signal_processor->accept(PrintRxProcParams());
        this->last_received_rf_sample = this->rx_rf->get_field();
        return this->rx_signal_processor->process_rx_signal(this->last_received_rf_sample);
    }

    double get_last_received_rf_sample() {
        return this->last_received_rf_sample;
    }
};
