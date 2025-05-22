import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_MOISTURE,
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_MOISTURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    ICON_WATER_PERCENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
)

DEPENDENCIES = ["i2c"]

somose_ns = cg.esphome_ns.namespace("somose")
SOMOSE = somose_ns.class_("SOMOSE", cg.PollingComponent, i2c.I2CDevice)
EnergyMode = somose_ns.enum("EnergyMode_t")
MoistureData = somose_ns.enum("Moisture_Data_t")

ENERGY_MODEs = {
    "CONTINOUS": EnergyMode.continous,
    "ENERGY_SAVING": EnergyMode.energy_saving,
}

MOISTURE_DATAs = {
    "LAST": MoistureData.last,
    "AVERAGE": MoistureData.average,
    "RAW": MoistureData.raw,
}

CONF_ENERGY_MODE = "energy_mode"
CONF_MOISTURE_DATA = "moisture_data"
CONF_REF_DRY = "ref_dry"    
CONF_REF_WET = "ref_wet"
CONF_FACTORY_RESET = "factory_reset"

# Neue Validierungsfunktion, die die Einheit für den Feuchtigkeitssensor anpasst
def validate_moisture_unit_conditional(config):
    # Überprüfen, ob ein Feuchtigkeitssensor in der Konfiguration definiert ist
    if CONF_MOISTURE in config:
        # Den konfigurierten Feuchtigkeitsdatenmodus abrufen (z.B. "LAST", "AVERAGE", "RAW")
        moisture_data_mode = config[CONF_MOISTURE_DATA]

        # Auf das Konfigurationswörterbuch des Feuchtigkeitssensors zugreifen
        moisture_sensor_config = config[CONF_MOISTURE]

        # Wenn der Feuchtigkeitsdatenmodus "RAW" ist, die Maßeinheit auf einen leeren String (einheitenlos) setzen
        if moisture_data_mode == "RAW":
            moisture_sensor_config['unit_of_measurement'] = ""
        # Andernfalls (für "LAST" oder "AVERAGE") sicherstellen, dass sie auf UNIT_PERCENT gesetzt ist
        else:
            moisture_sensor_config['unit_of_measurement'] = UNIT_PERCENT
    return config

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SOMOSE),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_MOISTURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,  # Standardwert, wird evtl. überschrieben
                accuracy_decimals=0,
                icon=ICON_WATER_PERCENT,
                device_class=DEVICE_CLASS_MOISTURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ENERGY_MODE, default="continous"): cv.enum(
                ENERGY_MODEs, upper=True, space="_"
            ),
            cv.Optional(CONF_MOISTURE_DATA, default="average"): cv.enum(
                MOISTURE_DATAs, upper=True, space="_"
            ),
            cv.Optional(CONF_REF_DRY, default=40): cv.uint16_t,
            cv.Optional(CONF_REF_WET, default=8000): cv.uint16_t,
            cv.Optional(CONF_FACTORY_RESET, default=False): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("20s"))
    .extend(i2c.i2c_device_schema(default_address=0x55)),
    validate_moisture_unit_conditional  # Hier statt .add_extra_validator()
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(var.set_temperature_sensor(sens))

    if moisture_config := config.get(CONF_MOISTURE):
        sens = await sensor.new_sensor(moisture_config)
        cg.add(var.set_moisture_sensor(sens))

    cg.add(var.set_Energy_Mode(config[CONF_ENERGY_MODE]))
    cg.add(var.set_Moisture_Data(config[CONF_MOISTURE_DATA]))
    cg.add(var.set_ref_dry(config[CONF_REF_DRY]))
    cg.add(var.set_ref_wet(config[CONF_REF_WET]))
    cg.add(var.set_factory_reset(config[CONF_FACTORY_RESET]))




# import esphome.codegen as cg
# from esphome.components import i2c, sensor
# import esphome.config_validation as cv
# from esphome.const import (
#     CONF_MOISTURE,
#     CONF_ID,
#     CONF_TEMPERATURE,
#     DEVICE_CLASS_MOISTURE,
#     DEVICE_CLASS_TEMPERATURE,
#     STATE_CLASS_MEASUREMENT,
#     ICON_WATER_PERCENT,
#     UNIT_CELSIUS,
#     UNIT_PERCENT,
# )

# DEPENDENCIES = ["i2c"]

# somose_ns = cg.esphome_ns.namespace("somose")
# SOMOSE = somose_ns.class_("SOMOSE", cg.PollingComponent, i2c.I2CDevice)
# EnergyMode = somose_ns.enum("EnergyMode_t")
# MoistureData = somose_ns.enum("Moisture_Data_t")

# ENERGY_MODEs = {
#   "CONTINOUS": EnergyMode.continous,
#   "ENERGY_SAVING": EnergyMode.energy_saving,
# }

# MOISTURE_DATAs = {
#     "LAST": MoistureData.last,
#     "AVERAGE": MoistureData.average,
#     "RAW": MoistureData.raw,
# }

# CONF_ENERGY_MODE = "energy_mode"
# CONF_MOISTURE_DATA = "moisture_data"
# CONF_REF_DRY = "ref_dry" 
# CONF_REF_WET = "ref_wet"
# CONF_FACTORY_RESET = "factory_reset"

# CONFIG_SCHEMA = (
#     cv.Schema(
#         {
#             cv.GenerateID(): cv.declare_id(SOMOSE),
#             cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_CELSIUS,
#                 accuracy_decimals=0,
#                 device_class=DEVICE_CLASS_TEMPERATURE,
#                 state_class=STATE_CLASS_MEASUREMENT,
#             ),
#             cv.Optional(CONF_MOISTURE): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_PERCENT,
#                 accuracy_decimals=0,
#                 icon=ICON_WATER_PERCENT,
#                 device_class=DEVICE_CLASS_MOISTURE,
#                 state_class=STATE_CLASS_MEASUREMENT,
#             ),
#             cv.Optional(CONF_ENERGY_MODE, default="continous"): cv.enum(
#             ENERGY_MODEs, upper=True, space="_"
#             ),
#             cv.Optional(CONF_MOISTURE_DATA, default="average"): cv.enum(
#             MOISTURE_DATAs, upper=True, space="_"
#             ),
#             cv.Optional(CONF_REF_DRY, default=40): cv.uint16_t,
#             cv.Optional(CONF_REF_WET, default=8000): cv.uint16_t,
#             cv.Optional(CONF_FACTORY_RESET, default=False): cv.boolean,
#         }
#     )
#     .extend(cv.polling_component_schema("20s"))
#     .extend(i2c.i2c_device_schema(default_address=0x55))
# )


# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)
#     await i2c.register_i2c_device(var, config)

#     if temperature_config := config.get(CONF_TEMPERATURE):
#         sens = await sensor.new_sensor(temperature_config)
#         cg.add(var.set_temperature_sensor(sens))

#     if moisture_config := config.get(CONF_MOISTURE):
#         sens = await sensor.new_sensor(moisture_config)
#         cg.add(var.set_moisture_sensor(sens))

#     cg.add(var.set_Energy_Mode(config[CONF_ENERGY_MODE]))
#     cg.add(var.set_Moisture_Data(config[CONF_MOISTURE_DATA]))
#      cg.add(var.set_ref_dry(config[CONF_REF_DRY]))
#     cg.add(var.set_ref_wet(config[CONF_REF_WET]))
#     cg.add(var.set_factory_reset(config[CONF_FACTORY_RESET]))
