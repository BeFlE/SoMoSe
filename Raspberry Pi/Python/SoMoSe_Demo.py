#!/usr/bin/python

import smbus
import time

#Define commands
setAddress = 0x41
setReferenceDry = 0x44
setReferenceWet = 0x55

getReferenceDry = 0x64
getReferenceWet = 0x75

getTemperature = 0x74
getSoilMoisture = 0x76
getRawData = 0x72

DEVICE_BUS = 1
bus = smbus.SMBus(DEVICE_BUS)

def ReadSoilMoisture(DEVICE_ADDR):
	SM = bus.read_i2c_block_data(DEVICE_ADDR,getSoilMoisture,2)
	return (SM)

def ReadTemperature(DEVICE_ADDR):
	T = bus.read_byte_data(DEVICE_ADDR,getTemperature)
	return (T)

def ReadRawData(DEVICE_ADDR):
	RD = bus.read_i2c_block_data(DEVICE_ADDR,getRawData,2)
	return (int.from_bytes(RD,byteorder='big'))

def ReadReferenceDry(DEVICE_ADDR):
	RD = bus.read_i2c_block_data(DEVICE_ADDR,getReferenceDry,2)
	return (int.from_bytes(RD,byteorder='big'))

def ReadReferenceWet(DEVICE_ADDR):
	RW = bus.read_i2c_block_data(DEVICE_ADDR,getReferenceWet,2)
	return (int.from_bytes(RW,byteorder='big'))

def WriteNewAddress(DEVICE_ADDR,NewAddress):
	bus.write_byte_data(DEVICE_ADDR,setAddress,(NewAddress << 1))
	time.sleep(0.2)
	return (NewAddress)

def WriteReferenceDry(DEVICE_ADDR,NewValue):
	bus.write_i2c_block_data(DEVICE_ADDR,setReferenceDry, [NewValue & 0xFF, (NewValue >> 8) & 0xFF])
	time.sleep(0.2)
	return (NewValue)

def WriteReferenceWet(DEVICE_ADDR,NewValue):
	bus.write_i2c_block_data(DEVICE_ADDR,setReferenceWet, [NewValue & 0xFF, (NewValue >> 8) & 0xFF])
	time.sleep(0.2)
	return (NewValue)

def FactoryReset(DEVICE_ADDR):
	bus.write_byte(DEVICE_ADDR,0x46)
	time.sleep(0.5)
	return (0x55)

def main():

	DEVICE_ADDR = 0x55
	
	#the following out-commented functions are only for illustration and have to be executed only if a value is to be changed

	#print ("reference- wet =", ReadReferenceWet(DEVICE_ADDR))
	#print ("reference- dry =", ReadReferenceDry(DEVICE_ADDR))

	#WriteReferenceWet(DEVICE_ADDR,5000)
	#WriteReferenceDry(DEVICE_ADDR,100)

	#print ("new reference- wet =", ReadReferenceWet(DEVICE_ADDR))
	#print ("new reference- dry =", ReadReferenceDry(DEVICE_ADDR))
	#print ("")
	#time.sleep(1)

	#DEVICE_ADDR=WriteNewAddress(DEVICE_ADDR,0x60)
	#print ("device- adress changed to:",DEVICE_ADDR)
	#print ("")
	#time.sleep(1)

	#DEVICE_ADDR= FactoryReset(DEVICE_ADDR)
	#print ("reset to factory values")
	#print ("device- adress:",DEVICE_ADDR)
	#print ("")
	
	time.sleep(2)#wait for SoMoSe to start

	while(True):
		Temp = ReadTemperature(DEVICE_ADDR)
		print ("temperature =", Temp,"°C")
		Moist = ReadSoilMoisture(DEVICE_ADDR)
		print ("average moisture =",round(Moist[0]/2.55,1),"%")
		print ("moisture =", round(Moist[1]/2.55,1),"%")
		print ("CTRL + C to end")
		print ("")
		time.sleep(2)

if __name__ == "__main__":
	main()
