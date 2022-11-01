#include "esphome.h"



class SoMoSe : public PollingComponent, public Sensor 
{
 public:
	Sensor *temperature_sensor = new Sensor();
	Sensor *moisture_sensor = new Sensor();
	int address;
  
	SoMoSe(int addr) : PollingComponent(10000) 
	{
		this->address = addr;
	}
	
	SoMoSe() : PollingComponent(10000) 
	{
		this->address = 0x55;
	}
	
	float get_setup_priority() const override { return esphome::setup_priority::BUS; }
	
	void setup() override 
	{
		// Initialize the device here. Usually Wire.begin() will be called in here,
		// though that call is unnecessary if you have an 'i2c:' entry in your config
		Wire.begin();
	}
	
	
	void update() override 
	{
		moisture_sensor->publish_state(getSensorValue(address)*100.0/256);
		temperature_sensor->publish_state(getTemperatureValue(address));
	}
	
	void setNewI2CAdr(int oldAdr, int newAdr) 
	{
		Wire.beginTransmission(byte(oldAdr)); 	// transmit to device
		Wire.write(byte(0x41));      		// command change address 
		Wire.write(byte((newAdr << 1) & 0xFE));   	// new address, 7-bit
		Wire.endTransmission();      		// stop transmitting
		this->address = newAdr;
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

		Wire.requestFrom(Adr, 2);    	// request 2 bytes from slave device 
		if (2 <= Wire.available()) 		// if two bytes were received
		{ 
			value = Wire.read();  	
				dump = Wire.read(); 
		}
		return value;
	}
	
	int getSensorValue(int Adr)
	{
		int value = 0;
		int dump;

		Wire.beginTransmission(byte(Adr)); 	// transmit to device
		Wire.write(byte(0x76));      		// command read value
		Wire.endTransmission();      		// stop transmitting

		delay(1);                   // maybe some delay is required

		Wire.requestFrom(Adr, 2);    	// request 2 bytes from slave device 
		if (2 <= Wire.available()) 		// if two bytes were received
		{ 
			dump = Wire.read();  	
			value = Wire.read(); 
		}
		return value;
	}
	
	char getTemperatureValue(int Adr) 
	{
		signed char value = 0;

		Wire.beginTransmission(byte(Adr));
		Wire.write(byte(0x74));
		Wire.endTransmission();

		delay(1);

		Wire.requestFrom(Adr, 1);    	// request 1 byte from slave device 
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

		Wire.requestFrom(Adr, 2);    	// request 2 bytes from slave device 
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

		Wire.requestFrom(Adr, 2);    	// request 2 bytes from slave device 
		if (2 <= Wire.available()) 		// if two bytes were received
		{ 
			value = Wire.read(); 
			value = value << 8;
				value |= Wire.read(); 
		}
		return value;
	}
};