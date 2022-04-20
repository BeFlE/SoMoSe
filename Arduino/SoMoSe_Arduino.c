
#include <Wire.h>
#include "SoMoSe_Arduino.h"

void setupI2C()  		//call once 
{
  	Wire.begin();           // join i2c bus as master
}

void setNewI2CAdr(int oldAdr, int newAdr)
{
	Wire.beginTransmission(byte(oldAdr)); 	// transmit to device
	Wire.write(byte(0x41));      		// command change address 
  	Wire.write(byte(newAdr & 0xFE));   	// new address, 7-bit
	Wire.endTransmission();      		// stop transmitting
}

void setReferenceDry(int Adr,int newValue)
{
	Wire.beginTransmission(byte(Adr)); 		// transmit to device
	Wire.write(byte(0x44));      			// command 
  	Wire.write(byte(newValue & 0xFF));   		// new Value, lower Byte
	Wire.write(byte((newValue & 0xFF00) >> 8));   	// new Value, higher Byte
	Wire.endTransmission();      			// stop transmitting
}

void setReferenceWet(int Adr,int newValue)
{
	Wire.beginTransmission(byte(Adr)); 		// transmit to device
	Wire.write(byte(0x55));      			// command 
  	Wire.write(byte(newValue & 0xFF));   		// new Value, lower Byte
	Wire.write(byte((newValue & 0xFF00) >> 8));   	// new Value, higher Byte
	Wire.endTransmission();      			// stop transmitting
}

int getAveragedSensorValue(int Adr)
{
	int value;
	int dump;

	Wire.beginTransmission(byte(Adr)); 	// transmit to device
	Wire.write(byte(0x76));      		// command read value
	Wire.endTransmission();      		// stop transmitting

	delay(1);                   // maybe some delay is required

	Wire.requestFrom(byte(Adr), 2);    	// request 2 bytes from slave device 
	if (2 <= Wire.available()) 		// if two bytes were received
	{ 
   		value = Wire.read();  	
    		dump = Wire.read(); 
	}
	return value;
}

int getSensorValue(int Adr)
{
	int value;
	int dump;

	Wire.beginTransmission(byte(Adr)); 	// transmit to device
	Wire.write(byte(0x76));      		// command read value
	Wire.endTransmission();      		// stop transmitting

	delay(1);                   // maybe some delay is required

	Wire.requestFrom(byte(Adr), 2);    	// request 2 bytes from slave device 
	if (2 <= Wire.available()) 		// if two bytes were received
	{ 
   		dump = Wire.read();  	
    		value = Wire.read(); 
	}
	return value;
}


char getTemperatureValue(int Adr)
{
	signed char value;

	Wire.beginTransmission(byte(Adr));
	Wire.write(byte(0x74));
	Wire.endTransmission();

	delay(1);

	Wire.requestFrom(byte(Adr), 1);    	// request 1 byte from slave device 
	if (1 <= Wire.available())
	{
    	value = Wire.read();
	}
	return value;
}

int getReferenceDry(int Adr)
{
	int value;

	Wire.beginTransmission(byte(Adr)); 	// transmit to device
	Wire.write(byte(0x64));      		// command read ReferenceDry
	Wire.endTransmission();      		// stop transmitting

	delay(1);                   // maybe some delay is required

	Wire.requestFrom(byte(Adr), 2);    	// request 2 bytes from slave device 
	if (2 <= Wire.available()) 		// if two bytes were received
	{ 
   		value = Wire.read(); 
 		value = value << 8;
    		value |= Wire.read(); 
	}
	return value;
}


int getReferenceWet(int Adr)
{
	int value;

	Wire.beginTransmission(byte(Adr)); 	// transmit to device
	Wire.write(byte(0x75));      		// command read ReferenceWet
	Wire.endTransmission();      		// stop transmitting

	delay(1);                   // maybe some delay is required

	Wire.requestFrom(byte(Adr), 2);    	// request 2 bytes from slave device 
	if (2 <= Wire.available()) 		// if two bytes were received
	{ 
   		value = Wire.read(); 
 		value = value << 8;
    		value |= Wire.read(); 
	}
	return value;
}
