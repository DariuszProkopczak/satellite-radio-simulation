#include <iostream>

using namespace std;

/* Value of electromagnetic field around earth */
// In order to reduce memory used, field will
// only be calculated at points where satellites
// are located.
class EMField {
    
    // The field at each receiver will be represented as
    // a vector of fields of all other transmitters
    // So to get the total field at a receiver, a summing
    // opration will be performed.
    vector<double> field;
    int num_sats;

public:
    EMField(int num_sats_in)
    {
        this->field.resize(num_sats_in*num_sats_in);

        for (int i = 0; i < (num_sats_in*num_sats_in); ++i)
            this->field[i] = 0;
        this->num_sats = num_sats_in;
    }

    void set_field (int rx_sat_id, int tx_sat_id, double field_value)
    {
        this->field[(rx_sat_id * this->num_sats) + tx_sat_id] = field_value;
        // cout << "Field Value: " << field_value;
        // cout << ", Electric Field Tx: ";
        // for (int i = 0; i < (this->num_sats * this->num_sats); ++i)
        //     cout << this->field[i] << ",";
        // cout << endl;
    }

    double get_field(int rx_sat_id) {
        // take the sum of fields of all transmitters
        double field_sum;
        for (int i = 0; i < this->num_sats; ++i)
            field_sum += this->field[(rx_sat_id * this->num_sats) + i];
        // cout << "Electric Field Rx";
        // for (int i = 0; i < (this->num_sats * this->num_sats); ++i)
        //     cout << this->field[i] << ",";
        // cout << endl;
        return field_sum;
    }
};
