#include "SoMoSe_Arduino.h"

void setup() {
  Serial.begin(9600);
  Serial.println("SoMoSe Demo");

  setupI2C();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  int hum = getAveragedSensorValue(int Adr);
  
  Serial.print("averaged humidity = ");
  Serial.println(hum);
}
