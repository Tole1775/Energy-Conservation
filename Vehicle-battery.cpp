#include <iostream>
#include <cmath>

#define TARGET_CAR // Define this in your build system (Makefile or CMake)
#include "AnnonVehicle.h"

enum class VehicleType { CAR, PLANE, BOAT };
enum class PowerSource { FUEL, ELECTRIC };

struct PhysicsState {
    double velocity;    // m/s
    double mass;        // kg
    double dragCoeff;   // Cd
    double airDensity;  // rho
};

class EnergyConserver {
public:
    /**
     * Calculates the "Golden Throttle" point where energy expenditure 
     * meets the highest distance-to-energy ratio.
     */
    double calculateOptimalThrottle(VehicleType v, PowerSource p, PhysicsState s) {
        double dragForce = 0.5 * s.airDensity * std::pow(s.velocity, 2) * s.dragCoeff;
        
        // Efficiency modifiers
        double efficiencyFactor = (p == PowerSource::ELECTRIC) ? 0.90 : 0.35;
        
        // Medium-specific resistance logic
        switch (v) {
            case VehicleType::BOAT:
                dragForce *= 1000; // Account for water density vs air
                break;
            case VehicleType::PLANE:
                // Lift-induced drag must be considered for planes
                dragForce += (s.mass * 9.81) / (s.velocity + 1.0);
                break;
            case VehicleType::CAR:
                // Rolling resistance
                dragForce += (s.mass * 9.81 * 0.015);
                break;
        }

        return dragForce / efficiencyFactor;
    }
};

int main() {
    AnnonCore::PowerState myCar = {0.15, true, false};
    // Automatically applies Car-specific logic
    AnnonCore::applyConservation(myCar);

    AnnonCore::PowerState myPlane = {0.15, true, false};
    // Automatically applies Plane-specific logic
    AnnonCore::applyConservation(myPlane);

    AnnonCore::PowerState myBoat = {0.15, true, false};
    // Automatically applies Boat-specific logic
    AnnonCore::applyConservation(myBoat);

    EnergyConserver ec;
    PhysicsState state = {30.0, 1500.0, 0.3, 1.225};
    double throttle = ec.calculateOptimalThrottle(VehicleType::CAR, PowerSource::ELECTRIC, state);
    std::cout << "Optimal throttle: " << throttle << std::endl;

    return 0;
}
