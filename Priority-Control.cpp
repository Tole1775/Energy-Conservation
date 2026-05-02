/* * Renewable Priority Battery Charger Control
 * Logic: Priority 1: Solar > Priority 2: Kinetic > Priority 3: Gas
 */

#include <Arduino.h> // Assuming microcontroller environment

// 1. Establish the Ranking Order (implicitly ordered 0, 1, 2)
enum ChargingPriority {
  PRIORITY_SOLAR,   // Rank 1 (highest)
  PRIORITY_KINETIC, // Rank 2
  PRIORITY_GAS      // Rank 3 (last resort)
};

// 2. Hardware Output Pins (Control Signals to Relays/MOSFETs)
// (Set according to your specific wiring)
const int PIN_SOLAR_ACTIVATE = 2;   // High enables Solar input
const int PIN_KINETIC_ACTIVATE = 3; // High enables Kinetic input
const int PIN_GAS_ACTIVATE = 4;     // High activates Generator start circuit

// 3. Sensor Input Pins (Analog Voltage Read on each input line)
// (Connect before the activation gates, via voltage dividers)
const int PIN_SOLAR_SENSOR = A0;   // Measures available Solar potential
const int PIN_KINETIC_SENSOR = A1; // Measures available Kinetic potential
const int PIN_GAS_SENSOR = A2;     // Measures if Generator is already running

void setup() {
  Serial.begin(115200);

  // Initialize all source control pins as outputs, starting OFF
  pinMode(PIN_SOLAR_ACTIVATE, OUTPUT);
  digitalWrite(PIN_SOLAR_ACTIVATE, LOW); 

  pinMode(PIN_KINETIC_ACTIVATE, OUTPUT);
  digitalWrite(PIN_KINETIC_ACTIVATE, LOW);

  pinMode(PIN_GAS_ACTIVATE, OUTPUT);
  digitalWrite(PIN_GAS_ACTIVATE, LOW);

  Serial.println("--- PRIORITY CHARGING SYSTEM ONLINE ---");
}

void loop() {
  // A. Analyze current energy conditions
  int availableSourcesCount = scanAvailableEnergy();

  // B. Run the priority logic if at least one source is present
  if (availableSourcesCount > 0) {
    applyPriorityControl();
  } else {
    // If no power is available, ensure everything is shutdown
    shutAllSourcesDown();
    Serial.println("SYSTEM STATUS: No input power detected. (Standby)");
  }

  // C. Basic cycle delay (e.g., 5 seconds)
  delay(5000); 
}

// ==========================================
// CORE PRIORITY LOGIC FUNCTION
// ==========================================
void applyPriorityControl() {
  // Use a sequential 'else if' structure to enforce ranking.
  // The first condition that is TRUE executes, and the rest are skipped.

  // Rank 1: Is Solar available?
  if (isSourceAvailable(PRIORITY_SOLAR)) {
    activateSolar();
    // Shunt (disable) other sources for efficiency and to prevent contention
    deactivateKinetic();
    deactivateGas();
    Serial.println("ACTIVE SOURCE: SOLAR (Priority 1)");
  }

  // Rank 2: Solar is out. Is Kinetic available?
  else if (isSourceAvailable(PRIORITY_KINETIC)) {
    activateKinetic();
    // Generator should not run if free energy is available
    deactivateGas(); 
    Serial.println("ACTIVE SOURCE: KINETIC (Priority 2, Solar Offline)");
  }

  // Rank 3: Solar AND Kinetic are both offline. Engage last resort.
  else {
    // (Ensure gas isn't activated instantly; may need battery voltage check first)
    activateGas(); 
    Serial.println("ACTIVE SOURCE: GAS GENERATOR (Last Resort)");
  }
}

// ==========================================
// SENSOR INTERFACE FUNCTIONS (Placeholder Logic)
// ==========================================

// Checks if a specific source has potential (e.g., >10V available)
bool isSourceAvailable(ChargingPriority source) {
  int sensorValue = 0;
  float thresholdVoltage = 10.0; // Minimal threshold for 'usable' power

  switch (source) {
    case PRIORITY_SOLAR:
      sensorValue = analogRead(PIN_SOLAR_SENSOR);
      // Example: 10V threshold on a 20V analog range (1024 steps)
      if (sensorValue > 512) return true; 
      break;

    case PRIORITY_KINETIC:
      sensorValue = analogRead(PIN_KINETIC_SENSOR);
      if (sensorValue > 512) return true;
      break;

    case PRIORITY_GAS:
      // Generator logic might differ (e.g., digital 'running' signal)
      // sensorValue = analogRead(PIN_GAS_SENSOR);
      // if (sensorValue > 512) return true;
      return true; // Simplified: assume gas always available if prioritized
      break;
  }
  return false;
}

int scanAvailableEnergy() {
  int count = 0;
  if (isSourceAvailable(PRIORITY_SOLAR)) count++;
  if (isSourceAvailable(PRIORITY_KINETIC)) count++;
  return count; // Assumes gas is available if needed
}

// ==========================================
// HARDWARE CONTROL FUNCTIONS
// ==========================================
void activateSolar() {
  digitalWrite(PIN_SOLAR_ACTIVATE, HIGH); // Example: Close Solar Relay
}

void activateKinetic() {
  digitalWrite(PIN_KINETIC_ACTIVATE, HIGH); // Example: Engage Turbine Input
}

void activateGas() {
  digitalWrite(PIN_GAS_ACTIVATE, HIGH); // Example: Signal Gen-Set to Start
}

void deactivateKinetic() {
  digitalWrite(PIN_KINETIC_ACTIVATE, LOW);
}

void deactivateGas() {
  digitalWrite(PIN_GAS_ACTIVATE, LOW);
}

void shutAllSourcesDown() {
  digitalWrite(PIN_SOLAR_ACTIVATE, LOW);
  digitalWrite(PIN_KINETIC_ACTIVATE, LOW);
  digitalWrite(PIN_GAS_ACTIVATE, LOW);
}

// Example of a simple delay timer for the Generator
unsigned int solarLossTimer = 0;
const unsigned int GAS_DELAY = 30000; // 30 seconds

if (isSourceAvailable(PRIORITY_SOLAR)) {
    solarLossTimer = millis(); // Reset timer
    activateSolar();
    deactivateGas();
} else if (millis() - solarLossTimer > GAS_DELAY) {
    // Only turn on gas if solar has been dead for 30 seconds
    activateGas();
}

// Logic: If Solar is gone AND Battery < 11.5V, then start Gas.
if (!isSourceAvailable(PRIORITY_SOLAR) && !isSourceAvailable(PRIORITY_KINETIC) && batteryVoltage < 11.5) {
    activateGas();
}

