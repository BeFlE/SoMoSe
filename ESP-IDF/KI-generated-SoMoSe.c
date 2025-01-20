//the code is converted with KI from arduino framework to ESP-IDF and has not yet been tested

#include "driver/i2c.h"
#include "esp_err.h"

// Define I2C parameters (same as previous code)
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ 100000 // 100 kHz

// Define I2C commands (same as previous code)
#define CMD_CHANGE_ADDR 0x41
#define CMD_SET_REFERENCE_DRY 0x44
#define CMD_SET_REFERENCE_WET 0x55
#define CMD_READ_VALUE 0x76
#define CMD_GET_TEMPERATURE 0x74
#define CMD_READ_REFERENCE_DRY 0x64
#define CMD_READ_REFERENCE_WET 0x75
#define CMD_GET_HW_VERSION 'h'
#define CMD_GET_FW_VERSION 'f'
#define CMD_GET_LOW_POWER_MODE 'o'
#define CMD_SET_LOW_POWER_MODE 'L'
#define CMD_START_MEASUREMENT 'M'
#define CMD_IS_MEASUREMENT_FINISHED 0x6f // 'o'

// Function prototypes
static esp_err_t i2c_master_init(void);
static esp_err_t i2c_master_write_data(uint8_t addr, uint8_t *data, size_t len);
static esp_err_t i2c_master_read_data(uint8_t addr, uint8_t *data, size_t len);

void setupI2C() {
    // Initialize I2C
    esp_err_t ret = i2c_master_init();
    if (ret != ESP_OK) {
        printf("Error initializing I2C: %s\n", esp_err_to_name(ret));
        return;
    }
}

void SoMoSe_setNewI2CAddr(int oldAddr, int newAddr) {
    // Prepare data to be written
    uint8_t data[3] = {CMD_CHANGE_ADDR, (newAddr << 1) & 0xFE};

    // Write data to device
    esp_err_t ret = i2c_master_write_data(oldAddr, data, sizeof(data));
    if (ret != ESP_OK) {
        printf("Error writing data to device: %s\n", esp_err_to_name(ret));
    }
}

void SoMoSe_setReferenceDry(int Addr, int newValue) {
    // Prepare data to be written
    uint8_t data[4] = {CMD_SET_REFERENCE_DRY, newValue & 0xFF, (newValue & 0xFF00) >> 8};

    // Write data to device
    esp_err_t ret = i2c_master_write_data(Addr, data, sizeof(data));
    if (ret != ESP_OK) {
        printf("Error writing data to device: %s\n", esp_err_to_name(ret));
    }
}

void SoMoSe_setReferenceWet(int Addr, int newValue) {
  // Prepare data to be written
  uint8_t data[4] = {CMD_SET_REFERENCE_WET, newValue & 0xFF, (newValue & 0xFF00) >> 8};

  // Write data to device
  esp_err_t ret = i2c_master_write_data(Addr, data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error writing data to device: %s\n", esp_err_to_name(ret));
  }
}

int SoMoSe_getAveragedSensorValue(int Addr) {
  uint8_t data[2];
  esp_err_t ret = i2c_master_read_data(Addr, CMD_READ_VALUE, data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error reading data from device: %s\n", esp_err_to_name(ret));
    return 0;
  }
  return data[0];
}

int SoMoSe_getRawSensorValue(int Addr) {
  uint8_t data[2];
  esp_err_t ret = i2c_master_read_data(Addr, 'r', data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error reading data from device: %s\n", esp_err_to_name(ret));
    return 0;
  }
  return (data[0] << 8) | data[1];
}

int SoMoSe_getSensorValue(int Addr) {
  // Same implementation as SoMoSe_getAveragedSensorValue
}

char SoMoSe_getTemperatureValue(int Addr) {
  uint8_t data;
  esp_err_t ret = i2c_master_read_data(Addr, CMD_GET_TEMPERATURE, &data, 1);
  if (ret != ESP_OK) {
    printf("Error reading data from device: %s\n", esp_err_to_name(ret));
    return 0;
  }
  return (signed char)data;
}

int SoMoSe_getReferenceDry(int Addr) {
  uint8_t data[2];

  // Read data from device
  esp_err_t ret = i2c_master_read_data(Addr, CMD_GET_REFERENCE_DRY, data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error reading reference dry value: %s\n", esp_err_to_name(ret));
    return 0;
  }

  // Combine bytes into integer
  return (data[0] << 8) | data[1];
}

int SoMoSe_getReferenceWet(int Addr) {
  uint8_t data[2];

  // Read data from device
  esp_err_t ret = i2c_master_read_data(Addr, CMD_GET_REFERENCE_WET, data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error reading reference wet value: %s\n", esp_err_to_name(ret));
    return 0;
  }

  // Combine bytes into integer
  return (data[0] << 8) | data[1];
}

float SoMoSe_getHwVersion(uint8_t Addr) {
  uint8_t data[4];

  // Send command and read data
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Addr << 1, true);
  i2c_master_write_byte(cmd, CMD_GET_HW_VERSION, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret != ESP_OK) {
    printf("Error getting HW version: %s\n", esp_err_to_name(ret));
    return 0.0f;
  }

  // Check for compatibility and read version data
  ret = i2c_master_read_data(Addr, 0, data, sizeof(data));
  if (ret != ESP_OK) {
    printf("Error reading HW version data: %s\n", esp_err_to_name(ret));
    return 0.0f;
  }

  if (data[0] != 'v') {
    return 2.2f; // Compatibility with SoMoSe v2.2
  }

  uint8_t major = data[1] - 0x30;
  if (data[2] != '.') {
    return 2.2f; // Compatibility with SoMoSe v2.2
  }
  uint8_t minor = data[3] - 0x30;

  return (float)(major + (float)minor / 10);
}
float SoMoSe_getFwVersion(uint8_t Addr) {
    uint8_t data[4];

    // Send command to get firmware version
    esp_err_t ret = i2c_master_write_data(Addr, &CMD_GET_FW_VERSION, 1);
    if (ret != ESP_OK) {
        return 0.0f;
    }

    // Check for transmission error
    if (i2c_master_read_data(Addr, data, 1) != ESP_OK) {
        return 1.2f;
    }
    if (data[0] != 'v') {
        return 1.2f;
    }

    // Read major and minor version numbers
    if (i2c_master_read_data(Addr, data, 2) != ESP_OK) {
        return 1.2f;
    }
    if (data[0] != '.') {
        return 1.2f;
    }

    uint8_t major = data[1] - 0x30;
    uint8_t minor = data[2] - 0x30;

    return (float)major + (float)minor / 10.0f;
}

bool SoMoSe_getLowPowerMode(uint8_t Addr) {
    uint8_t data[1];

    // Send command to get low power mode state
    esp_err_t ret = i2c_master_write_data(Addr, &CMD_GET_LOW_POWER_MODE, 1);
    if (ret != ESP_OK) {
        return false;
    }

    // Check for transmission error or no data available
    if (i2c_master_read_data(Addr, data, 1) != ESP_OK || data[0] == 0) {
        return false;
    }

    return (data[0] & 0x02) != 0;
}

void SoMoSe_setLowPowerMode(int Addr, bool turnOn) {
    uint8_t data = turnOn ? 1 : 0;

    // Send command to set low power mode
    i2c_master_write_data(Addr, &CMD_SET_LOW_POWER_MODE, 1);
    i2c_master_write_data(Addr, &data, 1);
    // No need to check return values here as these are simple writes
}

void SoMoSe_startMeasurement(int Addr, uint8_t Repetitions) {
    uint8_t data[2];
    data[0] = CMD_START_MEASUREMENT;
    data[1] = Repetitions;

    // Send command to start measurement
    i2c_master_write_data(Addr, data, sizeof(data));
    // No need to check return values here as this is a simple write
}

bool SoMoSe_isMeasurementFinished(uint8_t Addr) {
    uint8_t data[1];

    // Send command to get measurement status
    esp_err_t ret = i2c_master_write_data(Addr, &CMD_IS_MEASUREMENT_FINISHED, 1);
    if (ret != ESP_OK) {
        return false;
    }

    // Check for transmission error or no data available
    if (i2c_master_read_data(Addr, data, 1) != ESP_OK || data[0] == 0) {
        return false;
    }

    return (data[0] & 0x01) == 0; // 0 indicates measurement finished
}

// Private helper functions
static esp_err_t i2c_master_init(void) {
    // Configure I2C parameters
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ,
    };

    // Install and configure I2C driver
    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}

static esp_err_t i2c_master_write_data(uint8_t addr, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_master_read_data(uint8_t addr, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read(cmd, data + len - 1, 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
