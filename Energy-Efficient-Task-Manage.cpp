#define TARGET_CAR // Define this in your build system (Makefile or CMake)
#include "AnnonVehicle.h"

int main() {
    AnnonCore::PowerState myCar = {0.15, true, false};
    
    // Automatically applies Car-specific logic
    AnnonCore::applyConservation(myCar); 
    
    return 0;
}


#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

class EnergyManager {
private:
    std::mutex mtx;
    std::condition_variable cv;
    bool dataReady = false;

public:
    // Puts the thread into a deep sleep until hardware signals an interrupt
    void waitForSensorData() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this] { return dataReady; });
        dataReady = false; // Reset for next cycle
    }

    // Called by the peripheral (LiDAR/GPS) driver interrupt
    void signalDataArrival() {
        {
            std::lock_guard<std::mutex> lck(mtx);
            dataReady = true;
        }
        cv.notify_one();
    }

    void runEfficiently() {
        while (true) {
            waitForSensorData();
            
            // Process sensor fusion or navigation logic
            std::cout << "[SYSTEM] Processing burst data... Energy optimized." << std::endl;
            
            // Return to sleep immediately after processing
        }
    }
};
