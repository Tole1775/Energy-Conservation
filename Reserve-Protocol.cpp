bool checkSecurityReserve(double currentCapacity, PowerSource p) {
    // Reserve 15% for autonomous guidance and sensor fusion 
    // to ensure the vehicle can reach a "Safe-State" or execute protocols.
    double floor = (p == PowerSource::ELECTRIC) ? 0.15 : 0.10;
    return currentCapacity > floor;
}
