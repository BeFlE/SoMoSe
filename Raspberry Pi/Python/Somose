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

DEVICE_BUS = 1
bus = smbus.SMBus(DEVICE_BUS)

def ReadSoilMoisture(DEVICE_ADDR):
	SM = bus.read_i2c_block_data(DEVICE_ADDR,getSoilMoisture,2)
	return (SM)

def ReadTemperature(DEVICE_ADDR):
	T = bus.read_byte_data(DEVICE_ADDR,getTemperature)
	return (T)

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
	bus.write_word_data(DEVICE_ADDR,setReferenceDry,NewValue)
	time.sleep(0.2)
	return (NewValue)

def WriteReferenceWet(DEVICE_ADDR,NewValue):
	bus.write_word_data(DEVICE_ADDR,setReferenceWet,NewValue)
	time.sleep(0.2)
	return (NewValue)

def FactoryReset(DEVICE_ADDR):
	bus.write_byte(DEVICE_ADDR,0x46)
	time.sleep(0.5)
	return (0x55)
