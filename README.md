# Datasheets

[Datasheet SoMoSe v2.2](https://befle.de/Datasheets/SoMoSe_v3_4/Datenblatt_SoMoSe_v2.2.pdf)

[Datasheet SoMoSe v3.4](https://befle.de/Datasheets/SoMoSe_v3_4/Datenblatt_SoMoSe_v3.4.pdf)

# ESPHome Integration

Everything works with one sensor, please proceed with caution if several sensors are connected to the same bus

I am very grateful for reports of any problems or bugs.


## usage

```
external_components: 
  - source:
      type: git
      url: https://github.com/BeFlE/SoMoSe
      ref: main

i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true

sensor: 
  - platform: somose
    address: 0x55 #can be changed, sensor changes address automaticly if he is on default address(0x55),
                  #make sure that there are never two sensors with the same address on the bus at the same time
    temperature:
      name: "Temperature"
    moisture:
      name: "Moisture"
    energy_mode: "continous" #"energy_saving" --- optinal, default: continous
    moisture_data: "average" #"last","raw" --- optional, default: average
    ref_dry: 40  #optional
    ref_wet: 8000 #optional
    factory_reset: true #optional, resets sensor at "address" to factory-state (incl. I2C-address)

#multiple sensors on the same I2C-Bus, make sure that only one sensor is connected at a time during initial start-up (all sensors at default address 0x55),
#then the ESP is reset manually (so that the address is changed correctly) and then the next sensor is installed (then reset again)
  - platform: somose
    address: 0x55
    temperature:
      name: "Temperature 0x55"
    moisture:
      name: "Moisture 0x55"
    energy_mode: "energy_saving"
    moisture_data: "raw"
    ref_dry: 40
    ref_wet: 8000
```
