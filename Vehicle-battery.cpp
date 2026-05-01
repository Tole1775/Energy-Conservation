#include <iostream>
#include <cmath>

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
        double efficiencyFactor = (p == PowerSource.ELECTRIC) ? 0.90 : 0.35; 
        
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
