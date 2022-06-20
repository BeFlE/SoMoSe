#include <Wire.h>

void setupI2C()      //call once 
{
    Wire.begin();           // join i2c bus as master
}
int getAveragedSensorValue(int Adr)
{
  int value;
  int dump;

  Wire.beginTransmission(byte(Adr));  // transmit to device
  Wire.write(byte(0x76));         // command read value
  Wire.endTransmission();         // stop transmitting

  delay(1);                   // maybe some delay is required

  Wire.requestFrom(byte(Adr), 2);     // request 2 bytes from slave device 
  if (2 <= Wire.available())    // if two bytes were received
  { 
      value = Wire.read();    
        dump = Wire.read(); 
  }
  return value;
}

void setup() {
  Serial.begin(9600);
  Serial.println("SoMoSe Demo");

  setupI2C();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  int hum = getAveragedSensorValue(0x55);
  
  Serial.print("averaged humidity = ");
  Serial.println(hum);
}
