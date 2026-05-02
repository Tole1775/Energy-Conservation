// Energy Source ID Logic
enum PowerSource { SOLAR, KINETIC, GAS, UNKNOWN };

PowerSource identifySource(float voltage, float current) {
    // 1. Gas Generators usually provide a very steady, high-current bulk charge
    if (current > 10.0 && isStable(voltage)) {
        return GAS;
    }
    
    // 2. Solar has 'noise' from clouds and MPPT tracking adjustments
    if (voltage > 13.0 && hasLightFlicker(voltage)) {
        return SOLAR;
    }

    // 3. Kinetic (Wind/Hydro) often has RPM-based fluctuations
    if (voltage > 12.5 && current > 0.5) {
        return KINETIC;
    }

    return UNKNOWN;
}
