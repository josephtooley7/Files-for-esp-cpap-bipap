#include "HX711.h"

// Pin connections for HX711
const byte MPS_OUT_pin = 2; // DT pin
const byte MPS_SCK_pin = 3; // SCK pin

HX711 MPS20N0040D;

// Calibration constants (from your measurements)
const long OFFSET = 4584000;       // baseline raw value
const float SLOPE = 152000.0;      // counts per cmH2O

void setup() {
  Serial.begin(9600);
  MPS20N0040D.begin(MPS_OUT_pin, MPS_SCK_pin);
}

void loop() {
  // Take an average of 10 samples
  long avg_val = MPS20N0040D.read_average(10);

  // Convert raw value to cmH2O
  float pressure_cmH2O = (avg_val - OFFSET) / SLOPE;

  // Print results
  Serial.print("Raw: ");
  Serial.print(avg_val);
  Serial.print("  -> Pressure: ");
  Serial.print(pressure_cmH2O, 2); // 2 decimal places
  Serial.println(" cmH2O");

  delay(500); // half-second update
}
