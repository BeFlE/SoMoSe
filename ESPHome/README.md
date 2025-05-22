# ESPHome Integration

Everything works with one sensor, please proceed with caution if several sensors are connected to the same bus

I am very grateful for reports of any problems or bugs.


## usage

```
external_components: 
  - source:
      type: git
      url: https://github.com/BeFlE/SoMoSe/tree/main/ESPHome
      ref: main

i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true

sensor: 
  - platform: somose
    address: 0x55
    temperature:
      name: "Temperature"
    moisture:
      name: "Moisture"
```
