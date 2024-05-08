#include <Wire.h>
#include "SoMoSe_Arduino.h"

void setupI2C()   // call once
{
    Wire.begin();           // join i2c bus as master
}

void SoMoSe_setNewI2CAddr(int oldAddr, int newAddr)
{
    Wire.beginTransmission(byte(oldAddr)); // transmit to device
    Wire.write(byte(0x41));         // command change address
    Wire.write(byte((newAddr << 1) & 0xFE)); // new address, 7-bit
    Wire.endTransmission();         // stop transmitting
}

void SoMoSe_setReferenceDry(int Addr, int newValue)
{
    Wire.beginTransmission(byte(Addr));     // transmit to device
    Wire.write(byte(0x44));            // command
    Wire.write(byte(newValue & 0xFF));       // new Value, lower Byte
    Wire.write(byte((newValue & 0xFF00) >> 8));  // new Value, higher Byte
    Wire.endTransmission();             // stop transmitting
}

void SoMoSe_setReferenceWet(int Addr, int newValue)
{
    Wire.beginTransmission(byte(Addr));     // transmit to device
    Wire.write(byte(0x55));            // command
    Wire.write(byte(newValue & 0xFF));       // new Value, lower Byte
    Wire.write(byte((newValue & 0xFF00) >> 8));  // new Value, higher Byte
    Wire.endTransmission();             // stop transmitting
}

int SoMoSe_getAveragedSensorValue(int Addr)
{
    int value;
    int dump;

    Wire.beginTransmission(byte(Addr)); // transmit to device
    Wire.write(byte(0x76));          // command read value
    Wire.endTransmission();          // stop transmitting

    delay(1);                   // maybe some delay is required

    Wire.requestFrom(byte(Addr), (byte)2);    // request 2 bytes from slave device
    if (2 <= Wire.available())         // if two bytes were received
    {
        value = Wire.read();
        dump = Wire.read();
    }
    return value;
}

int SoMoSe_getSensorValue(int Addr)
{
    int value;
    int dump;

    Wire.beginTransmission(byte(Addr)); // transmit to device
    Wire.write(byte(0x76));          // command read value
    Wire.endTransmission();          // stop transmitting

    delay(1);                   // maybe some delay is required

    Wire.requestFrom(byte(Addr), (byte)2);    // request 2 bytes from slave device
    if (2 <= Wire.available())         // if two bytes were received
    {
        dump = Wire.read();
        value = Wire.read();
    }
    return value;
}


char SoMoSe_getTemperatureValue(int Addr)
{
    signed char value;

    Wire.beginTransmission(byte(Addr));
    Wire.write(byte(0x74));
    Wire.endTransmission();

    delay(1);

    Wire.requestFrom(byte(Addr), (byte)1);    // request 1 byte from slave device
    if (1 <= Wire.available())
    {
        value = Wire.read();
    }
    return value;
}

int SoMoSe_getReferenceDry(int Addr)
{
    int value;

    Wire.beginTransmission(byte(Addr)); // transmit to device
    Wire.write(byte(0x64));          // command read ReferenceDry
    Wire.endTransmission();          // stop transmitting

    delay(1);                   // maybe some delay is required

    Wire.requestFrom(byte(Addr), (byte)2);    // request 2 bytes from slave device
    if (2 <= Wire.available())         // if two bytes were received
    {
        value = Wire.read();
        value = value << 8;
        value |= Wire.read();
    }
    return value;
}


int SoMoSe_getReferenceWet(int Addr)
{
    int value;

    Wire.beginTransmission(byte(Addr)); // transmit to device
    Wire.write(byte(0x75));          // command read ReferenceWet
    Wire.endTransmission();          // stop transmitting

    delay(1);                   // maybe some delay is required

    Wire.requestFrom(byte(Addr), (byte)2);    // request 2 bytes from slave device
    if (2 <= Wire.available())         // if two bytes were received
    {
        value = Wire.read();
        value = value << 8;
        value |= Wire.read();
    }
    return value;
}

// only for Somose versions > v3.4 (HW version)

float SoMoSe_getHwVersion(uint8_t Addr)
{
    Wire.beginTransmission(byte(Addr));
    Wire.write('h');
    if (Wire.endTransmission())
    {
        return 0;
    }
    delay(1);                   // maybe some delay is required
    Wire.requestFrom(byte(Addr), (byte) 4);
    if (4 <= Wire.available())
    {
        if (Wire.read() != 'v') // Compatibility with SoMoSe v2.2
            return 2.2;
        uint8_t Major = Wire.read() - 0x30;
        if (Wire.read() != '.') // Compatibility with SoMoSe v2.2
            return 2.2;
        uint8_t Minor = Wire.read() - 0x30;
        return (float)(Major + (float)Minor / 10);
    }
    return 0;
}
float SoMoSe_getFwVersion(uint8_t Addr)
{
    Wire.beginTransmission(byte(Addr));
    Wire.write('f');
    if (Wire.endTransmission())
    {
        return 0;
    }
    delay(1);                   // maybe some delay is required
    Wire.requestFrom(byte(Addr), (byte) 4);
    if (4 <= Wire.available())
    {
        if (Wire.read() != 'v')
            return 1.2;
        uint8_t Major = Wire.read() - 0x30;
        if (Wire.read() != '.')
            return 1.2;
        uint8_t Minor = Wire.read() - 0x30;
        return (float)(Major + (float)Minor / 10);
    }
    return 0;
}
bool SoMoSe_getLowPowerMode(uint8_t Addr)
{
    Wire.beginTransmission(byte(Addr));
    Wire.write('o');
    if (Wire.endTransmission())
    {
        return 0;
    }
    delay(1);                   // maybe some delay is required
    Wire.requestFrom(byte(Addr), (byte) 1);
    if (1 <= Wire.available())
    {
        if (Wire.read() & 0x02)
            return 1;
        else
            return 0;
    }
    return 0;
}
void SoMoSe_setLowPowerMode(int Addr, bool turnOn)
{
    Wire.beginTransmission(byte(Addr));
    Wire.write('L');
    Wire.write(byte(turnOn));
    Wire.endTransmission();
    delay(25);
}
void SoMoSe_startMeasurement(int Addr, uint8_t Repetitions) // a measurement must be triggered in low power mode. the data can be read at any time. If no measurement is triggered, old data is read!
{                                                           // the number of repetitions is a tradeoff between accuracy and energy consumption. A value of at least 100 is recommended
    Wire.beginTransmission(byte(Addr));            	    // After starting the measurement, wait at least 250ms and check "SoMoSe_isMeasurementFinished()" before reading the data
    Wire.write('M');
    Wire.write(byte(Repetitions));
    Wire.endTransmission();
}
bool SoMoSe_isMeasurementFinished(uint8_t Addr)
{
    Wire.beginTransmission(byte(Addr));
    Wire.write('o');
    if (Wire.endTransmission())
    {
        return 0;
    }
    delay(1);                   // maybe some delay is required
    Wire.requestFrom(byte(Addr), (byte) 1);
    if (1 <= Wire.available())
    {
        if (Wire.read() & 0x01)
            return 0; // measuring
        else
            return 1; // finished
    }
    return 0;
}
```
