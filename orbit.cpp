#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>

using namespace std;

// will store current 3D position of every satellite
class SatellitePositions {

    // Vector of satellite positions.
    // Contains x, y, z values for each
    // satellite.
    vector<double> sat_pos;

    // number of total satellites
    int num_sats;

public:

    SatellitePositions(int num_sats_in) {
        this->sat_pos.resize(3 * num_sats_in);
        this->num_sats = num_sats_in;
    }

    void set_position(int sat_id, double x, double y, double z) {
    // set position of satellite sat_id to (x, y, z)    
        this->sat_pos[(3 * sat_id)] = x;
        this->sat_pos[(3 * sat_id) + 1] = y;
        this->sat_pos[(3 * sat_id) + 2] = z;
    }

    void update_position(int sat_id, double dx, double dy, double dz) {
        // change position of satellite sat_id by (dx, dy, dz)
        this->sat_pos[(3 * sat_id)] += dx;
        this->sat_pos[(3 * sat_id) + 1] += dy;
        this->sat_pos[(3 * sat_id) + 2] += dz;
    }

    tuple<double, double, double> get_position(int sat_id) {
        // get position vector of satellite
        double x = this->sat_pos[(3 * sat_id)];
        double y = this->sat_pos[(3 * sat_id) + 1];
        double z = this->sat_pos[(3 * sat_id) + 2];
        return tuple<double, double, double>{x, y, z};
    }

    // calculate the distance between two satellites
    double calc_distance(int sat_id_1, int sat_id_2) {
        double x1 = this->sat_pos[(3 * sat_id_1)];
        double y1 = this->sat_pos[(3 * sat_id_1) + 1];
        double z1 = this->sat_pos[(3 * sat_id_1) + 2];
        double x2 = this->sat_pos[(3 * sat_id_2)];
        double y2 = this->sat_pos[(3 * sat_id_2) + 1];
        double z2 = this->sat_pos[(3 * sat_id_2) + 2];

        return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
    }

    int get_num_sats() {
        return this->num_sats;
    }
};

double constexpr calc_exp(double base, int power) {
    // used to calculate constants during compile time
    double ret = 1;
    if (power < 0)
    {
        power *= -1;
        ret = 1 / base;
        for (int i = 1; i < power; ++i)
        {
            ret = ret/base;
        }
    }
    else if (power > 0) {
        ret = base;
        for (int i = 0; i < power; ++i)
        {
            ret *= base;
        }
    }

    return base;
}

// Used to get a random velocity vector that is tangential to a a point on
// a sphere concentric with the earth.
tuple<double, double, double> get_random_tangential_velocity(double r, double a, double b, double c) {

    double v_x, v_y, v_z;
    double constexpr G_M_Earth = 3.986004418 * calc_exp(10, 14); // Gravitational Parameter of Earth , in m^3 / s^2
    double v_orbit = sqrt(G_M_Earth / r);
 
    // generate random direction (x, y, z)
    if (((double) rand() / RAND_MAX) < 0.5) {
        // random y and z, solve for x
        v_y = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_z = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_x = -((b * v_y + c * v_z) / a);
    }
    else if (((double) rand() / RAND_MAX) < 0.5) {
        // random x and z, solve for y
        v_x = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_z = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_y = -((a * v_x + c * v_z) / b);
    } 
    else {
        // random x and y, solve for z
        v_x = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_y = (double) rand() / RAND_MAX * sqrt(v_orbit / 3);
        v_z = -((a * v_x + b * v_y) / c);
    }

    // now scale the vector so the velocity magnitude 
    // is appropriate for orbit using equation:
    // v = sqrt((G*M) / r)
    double curr_magnitude = sqrt(v_x*v_x + v_y*v_y + v_z*v_z);
    v_x = v_x * (v_orbit / curr_magnitude);
    v_y = v_y * (v_orbit / curr_magnitude);
    v_z = v_z * (v_orbit / curr_magnitude); 

    return tuple<double, double, double>{v_x, v_y, v_z};
}

// calculate gravity vector based on satellite x, y, z position
tuple<double, double, double> calc_gravity(tuple<double, double, double> sat_position) {
    double constexpr G_M_Earth = 3.986004418 * calc_exp(10, 14); // Gravitational Parameter of Earth , in m^3 / s^2

    double x = -1 * get<0>(sat_position);
    double y = -1 * get<1>(sat_position);
    double z = -1 * get<2>(sat_position);
    double curr_magnitude = sqrt(x * x + y * y + z * z);

    double g = G_M_Earth / (x * x + y * y + z * z);
    // double g = 9.81;

    double g_x = x * (g / curr_magnitude);
    double g_y = y * (g / curr_magnitude);
    double g_z = z * (g / curr_magnitude);

    return tuple<double, double, double>{g_x, g_y, g_z};
}