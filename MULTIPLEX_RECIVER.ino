//Reciver

#include <Arduino.h>

#define RXD2 16
#define TXD2 17
#define BAUD 9600

// Multiplex pins wired from sender
const int pinA = 12;
const int pinB = 13;
const int pinC = 14;

String decodeLabel(int a, int b, int c) {
  if (a==1 && b==0 && c==1) return "Pressure";
  if (a==0 && b==0 && c==1) return "Humidity";
  if (a==0 && b==1 && c==1) return "Temp";
  if (a==1 && b==0 && c==0) return "DallasTemp";
  return "Unknown";
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(BAUD, SERIAL_8N1, RXD2, TXD2);

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);

  Serial.println("Receiver initialized. Listening for 00 values with multiplex pins...");
}

void loop() {
  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    incoming.trim();

    int a = digitalRead(pinA);
    int b = digitalRead(pinB);
    int c = digitalRead(pinC);
    String label = decodeLabel(a,b,c);

    if (incoming.length() == 2 && isDigit(incoming[0]) && isDigit(incoming[1])) {
      int value = incoming.toInt();
      Serial.print(label);
      Serial.print(" (");
      Serial.print(a); Serial.print(","); Serial.print(b); Serial.print(","); Serial.print(c);
      Serial.print(") = ");
      Serial.println(value);
    } else {
      Serial.print("Raw incoming: ");
      Serial.println(incoming);
    }
  }
}
