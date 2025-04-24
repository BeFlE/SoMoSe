#include "somose.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace somose {

static const char *const TAG = "somose";

  void SOMOSE::setup() {
    ESP_LOGCONFIG(TAG, "Setting up SOMOSE...");
  
    uint8_t command = 0x64;
		uint8_t value[2];

		if (this->write(&command, 1) != i2c::ERROR_OK) { // command read value
      ESP_LOGE(TAG, "Write failed!");
      this->status_set_warning();
      return;
    }

   	delay(1);                   // maybe some delay is required

    if (this->read(value, 2) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read Value failed!");
      this->status_set_warning();
      return;
    }

    ESP_LOGD(TAG, "Read moisture min value %d (%d, %d).", value[0] * 256 + value[1], value[0], value[1]);
		this->moisture_min_ = value[0] * 256 + value[1];

    command = 0x75;

		if (this->write(&command, 1) != i2c::ERROR_OK) { // command read value
      ESP_LOGE(TAG, "Write failed!");
      this->status_set_warning();
      return;
    }

   	delay(1);                   // maybe some delay is required

    if (this->read(value, 2) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read Value failed!");
      this->status_set_warning();
      return;
    }

    ESP_LOGD(TAG, "Read moisture max value %d (%d, %d).", value[0] * 256 + value[1], value[0], value[1]);
		this->moisture_max_ =  value[0] * 256 + value[1];

    // get status
    /* uint8_t status = 0;
    if (this->read(&status, 1) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read failed!");
      this->mark_failed();
      return;
    }
  
    // reset registers if required, according to the datasheet
    // this can be required after power on, although this was
    // never required during testing
    if ((status & 0x18) != 0x18) {
      ESP_LOGD(TAG, "Resetting SOMOSE registers");
      if (!this->reset_register_(0x1B)) {
        this->mark_failed();
        return;
      }
      if (!this->reset_register_(0x1C)) {
        this->mark_failed();
        return;
      }
      if (!this->reset_register_(0x1E)) {
        this->mark_failed();
        return;
      }
    }
    */
  }
  
 	uint8_t SOMOSE::getSensorValue_()
	{
    uint8_t command = 0x76;
		uint8_t value = 0;
		uint8_t dump;

		if (this->write(&command, 1) != i2c::ERROR_OK) { // command read value
      ESP_LOGE(TAG, "Write failed!");
      this->status_set_warning();
      return 0;
    }

   	delay(1);                   // maybe some delay is required

    if (this->read(&dump, 1) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read dump failed!");
      this->status_set_warning();
      return 0;
    }

    if (this->read(&value, 1) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read moisture failed!");
      this->status_set_warning();
      return 0;
    }
	
		return value;
	}

   	uint16_t SOMOSE::getSensorRAWValue_()
	{
    uint8_t command = 0x72;
		uint8_t value[2];

		if (this->write(&command, 1) != i2c::ERROR_OK) { // command read value
      ESP_LOGE(TAG, "Write failed!");
      this->status_set_warning();
      return 0;
    }

   	delay(1);                   // maybe some delay is required

    if (this->read(value, 2) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read Value failed!");
      this->status_set_warning();
      return 0;
    }

     ESP_LOGD(TAG, "Read moisture raw value %d (%d, %d).", value[0] * 256 + value[1], value[0], value[1]);
		return value[0] * 256 + value[1];
	}
	
	uint8_t SOMOSE::getTemperatureValue_() 
	{
    uint8_t command = 0x74;
		uint8_t value = 0;

		if (this->write(&command, 1) != i2c::ERROR_OK) { // command read value
      ESP_LOGE(TAG, "Write failed!");
      this->status_set_warning();
      return 0;
    }

   	delay(1);                   // maybe some delay is required

    if (this->read(&value, 1) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read temperature failed!");
      this->status_set_warning();
      return 0;
    }
		return value;
	}

  void SOMOSE::update() {
    // request measurement

    ESP_LOGD(TAG, "SOMOSE::update");

    float temperature = getTemperatureValue_() * 1.0;
    float moisture = getSensorRAWValue_()*100.0/8000;

    if (this->temperature_sensor_ != nullptr) {
      this->temperature_sensor_->publish_state(temperature);
    }
    if (this->moisture_sensor_ != nullptr) {
      this->moisture_sensor_->publish_state(moisture);
    }
    
    this->status_clear_warning();
  }
  
  void SOMOSE::dump_config() {
    ESP_LOGCONFIG(TAG, "SOMOSE:");
    LOG_I2C_DEVICE(this);
    if (this->is_failed()) {
      ESP_LOGE(TAG, "Communication with SOMOSE failed!");
    }
    LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
    LOG_SENSOR("  ", "moisture", this->moisture_sensor_);
  }
  
  float SOMOSE::get_setup_priority() const { return setup_priority::DATA; }


}  // namespace somose
}  // namespace esphome
