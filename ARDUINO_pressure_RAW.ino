const int dataPin = 2; // HX710B OUT
const int clockPin = 3; // HX710B SCK

void setup() {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  Serial.begin(9600);
}

long readHX710B() {
  long result = 0;
  // Wait until sensor is ready (OUT goes LOW)
  while (digitalRead(dataPin) == HIGH);

  // Read 24 bits
  for (int i = 0; i < 24; i++) {
    digitalWrite(clockPin, HIGH);
    result = (result << 1) | digitalRead(dataPin);
    digitalWrite(clockPin, LOW);
  }

  // Extra clock pulse to set gain/channel
  digitalWrite(clockPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(clockPin, LOW);

  return result;
}

void loop() {
  long pressureRaw = readHX710B();
  Serial.print("Pressure raw value: ");
  Serial.println(pressureRaw);
  delay(500);
}
