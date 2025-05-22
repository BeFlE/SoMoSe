# ESPHome Integration

first try --> PLEASE USE WITH CARE!

just the temperature and moisture readout is working, nothing else.


## usage

```
external_components: 
  - source:
      type: git
      url: https://github.com/joe16joe16/SoMoSe
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
