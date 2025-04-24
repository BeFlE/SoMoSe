#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace somose {

class SOMOSE : public PollingComponent, public i2c::I2CDevice {
    public:
        void dump_config() override;
        void update() override;
        void setup() override;
        float get_setup_priority() const override;
    
        void set_temperature_sensor(sensor::Sensor *temperature_sensor) { this->temperature_sensor_ = temperature_sensor; }
        void set_moisture_sensor(sensor::Sensor *moisture_sensor) { this->moisture_sensor_ = moisture_sensor; }
    
    protected:
        uint8_t getSensorValue_();
        uint16_t getSensorRAWValue_();
        uint8_t getTemperatureValue_();
        sensor::Sensor *temperature_sensor_{nullptr};
        sensor::Sensor *moisture_sensor_{nullptr};
        uint16_t moisture_min_ = 0;
        uint16_t moisture_max_ = 0;
    };
    
}  // namespace somose
}  // namespace esphome
