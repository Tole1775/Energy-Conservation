#ifndef ANNON_VEHICLE_H
#define ANNON_VEHICLE_H

// --- Environment Detection ---
#if defined(TARGET_CAR)
    #define VEHICLE_TYPE "LAND"
    #define MAX_RPM 7000
    #define FUEL_RESERVE_LITERS 5.0
#elif defined(TARGET_PLANE)
    #define VEHICLE_TYPE "AERIAL"
    #define CRITICAL_ALTITUDE 500 // meters
    #define STALL_SPEED 45 // knots
#elif defined(TARGET_BOAT)
    #define VEHICLE_TYPE "AQUATIC"
    #define HULL_SPEED_CONSTANT 1.34
#endif

// --- Energy Logic ---
#include <iostream>

namespace AnnonCore {

    struct PowerState {
        double currentLevel;
        bool isElectric;
        bool lowPowerMode;
    };

    /**
     * Automatic Energy Conservation Protocol
     * Implements logic for Car, Plane, or Boat based on compile targets.
     */
    inline void applyConservation(PowerState& state) {
        if (state.currentLevel < 0.20) {
            state.lowPowerMode = true;
            #ifdef TARGET_PLANE
                std::cout << "[ALARM] Low Fuel/Charge. Prioritizing Glide Path." << std::endl;
            #elif defined(TARGET_CAR)
                std::cout << "[INFO] Limiting Torque to extend range." << std::endl;
            #endif
        }
    }

    /**
     * GPS Interface Integration
     * Pulls data from Annon-Mobile telemetry port.
     */
    struct GPSPayload {
        double lat, lon, alt;
        void sync() {
            // Integration logic with your GPS Receiver code
        }
    };
}

#endif // ANNON_VEHICLE_H
