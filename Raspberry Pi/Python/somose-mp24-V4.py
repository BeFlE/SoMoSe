#!/usr/bin/python
# -*- coding: utf-8 -*-
#     Script@mp24 23.03.2022

import smbus2
import time
import os, sys

smbus = smbus2

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

DEVICE_ADDR = 0x55   # i2c Adresse kann mit somose-demo.py geändert werden

#WriteReferenceWet(DEVICE_ADDR,2200) # Kalibrierung 100% Hoeherer Wert veringert 100 %
#WriteReferenceDry(DEVICE_ADDR,160)  # Kalibrierung   0%

# Define delay between readings
delay = 2 # alle X Sekunden Messen

 
z = 1
while z < 11:     
    Temp = ReadTemperature(DEVICE_ADDR) # bfs Temperatur einlesen
    m = ReadSoilMoisture(DEVICE_ADDR)   # m = Hilfvariable bfs Feuchte einlesen
    M = round(m[1]/2.55,1)              # Bodenfeuchte aus m
    i = round(m[0]/2.55,1)              # Mittelwert Bodenfeuchte      
    # Wait before repeating loop
    time.sleep(delay)      
    z = z + 1   

# Output in einzelne Dateien schreiben temp=Temperatur wet=Feuchte mwet=Feuchte-Mittelwert Pfad kann angepasst werden
output = "{T1}".format(
                      T1 = Temp ,                   
)
File = open ("/home/pi/bfs1.temp","w")
File.write(output)
File.close()
time.sleep(2)
output = "{W1}".format(
                      W1 = M ,
)
File = open ("/home/pi/bfs1.wet","w")
File.write(output)
File.close()
time.sleep(2)
output = "{M1}".format(
                      M1 = i ,
)
File = open ("/home/pi/bfs1.mwet","w")
File.write(output)
File.close()
time.sleep(2)
print("Habe fertig")
