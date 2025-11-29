//TRansmit


#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "HX711.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// ==== AHT10 setup ====
Adafruit_AHTX0 aht;

// ==== HX711 setup ====
const byte MPS_OUT_pin = 4;
const byte MPS_SCK_pin = 5;
HX711 MPS20N0040D;

// Calibration constants
const long  OFFSET = 4584000L;
const float SLOPE  = 152000.0f;

// ==== Gate pin ====
const int gatePin = 15;

// ==== Multiplex pins ====
const int pinA = 12;
const int pinB = 13;
const int pinC = 14;

// ==== Dallas Temp (DS18B20) ====
#define ONE_WIRE_BUS 25
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables
float humidityVal = 0.0;
float tempVal = 0.0;
float pressureVal = 0.0;
float dallasTemp = 0.0;

#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(gatePin, INPUT_PULLDOWN);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);

  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 sensor error!");
    while (1);
  }

  MPS20N0040D.begin(MPS_OUT_pin, MPS_SCK_pin);
  sensors.begin();

  Serial.println("System initialized: slower transmit, 00 format with multiplex pins.");
}

void loop() {
  if (digitalRead(gatePin) == HIGH) {
    delay(100);
    return;
  }

  // --- Pressure (throttled to 5 Hz) ---
  static unsigned long lastPressure = 0;
  if (millis() - lastPressure >= 50) {
    long raw = MPS20N0040D.read_average(1);
    pressureVal = (raw - OFFSET) / SLOPE;
    sendValue(pressureVal, 1,0,1,"Pressure");
    lastPressure = millis();
  }

  // --- Humidity/Temperature/Dallas (1 Hz) ---
  static unsigned long lastAHT = 0;
  if (millis() - lastAHT >= 2500) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    humidityVal = humidity.relative_humidity;
    tempVal = temp.temperature;
    sendValue(humidityVal, 0,0,1,"Humidity");
    sendValue(tempVal,     0,1,1,"Temp");

    sensors.requestTemperatures();
    dallasTemp = sensors.getTempCByIndex(0);
    sendValue(dallasTemp, 1,0,0,"DallasTemp");

    lastAHT = millis();
  }
}

void sendValue(float rawValue, int a, int b, int c, const char* label) {
  int valueOut = (int)(rawValue + 0.5f);
  if (valueOut < 0) valueOut = 0;
  if (valueOut > 99) valueOut = valueOut % 100;

  char formatted[3];
  sprintf(formatted, "%02d", valueOut);

  digitalWrite(pinA, a);
  digitalWrite(pinB, b);
  digitalWrite(pinC, c);

  Serial.print(label);
  Serial.print(" (");
  Serial.print(a); Serial.print(b); Serial.print(c);
  Serial.print(") → ");
  Serial.println(formatted);

  Serial2.println(formatted);   // just "00".."99"
  delay(5);                     // small pacing delay
}
