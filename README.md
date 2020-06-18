# satellite-radio-simulation
Simulation of satellite orbit and radio communication. Orbits of satellites are simulated along with RF communication between satellites. User can select between AM and FM modulation techniques. Value of input and output signal amplitudes along with satellite positions are printed at each time step.

More configurable parameters are in main.cpp.

Note that the project is near completion but still requires a few bug fixes.

# Build and run
Can build using command similar to:

clang++ -I path/to_repo main.cpp -std=c++17 -o satellite
 
can run like: "./satellite AM" or "./satellite FM"
  
# Example

```
./satellite "AM"

Running Version #: 1
Version Name: Version 1 6/14/2020
Time Step: 0
    Transmitted Audio Sample: 0
    Transmit Satellite Position: 
        r: 8.357e+06 meters , rho: -14.2624 degrees, theta: 0.0197225 degrees 
    Transmitted RF Sample: 0
    Receive Satellite Position: 
        r: 8.357e+06 meters , rho: 59.2762 degrees, theta: 168.223 degrees 
    Received RF Sample: 6.93052e-310
    Received Audio Sample: 0
Time Step: 1
    Transmitted Audio Sample: 125.66
    Transmit Satellite Position: 
        r: 8.357e+06 meters , rho: -14.2624 degrees, theta: 0.0197225 degrees 
    Transmitted RF Sample: 24.4268
    Receive Satellite Position: 
        r: 8.357e+06 meters , rho: 59.2762 degrees, theta: 168.223 degrees 
    Received RF Sample: 6.93052e-310
    Received Audio Sample: 3.85534e-309
    
...
```
