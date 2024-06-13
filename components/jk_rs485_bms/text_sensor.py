import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, CONF_PASSWORD, ICON_EMPTY, ICON_TIMELAPSE

from . import CONF_JK_RS485_BMS_ID, JK_RS485_BMS_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_rs485_bms"]

CODEOWNERS = ["@syssi","@txubelaxu"]

CONF_BATTERY_TYPE = "battery_type"
CONF_ERRORS = "errors"
CONF_OPERATION_STATUS = "operation_status"
CONF_DEVICE_TYPE = "device_type"
CONF_SOFTWARE_VERSION = "software_version"
CONF_MANUFACTURER = "manufacturer"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"
CONF_NETWORK_NODES_AVAILABLE = "network_nodes_available"

CONF_INFO_VENDORID = "info_vendorid"
CONF_INFO_HARDWARE_VERSION = "info_hardware_version";
CONF_INFO_SOFTWARE_VERSION = "info_software_version";
CONF_INFO_DEVICE_NAME = "info_device_name";
CONF_INFO_DEVICE_PASSWORD = "info_device_password";
CONF_INFO_DEVICE_SERIAL_NUMBER = "info_device_serial_number";
CONF_INFO_DEVICE_SETUP_PASSCODE = "info_device_setup_passcode";

ICON_BATTERY_TYPE = "mdi:car-battery"
ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_MODE = "mdi:heart-pulse"
ICON_PASSWORD = "mdi:lock-outline"
ICON_NETWORK = "mdi:lan"
ICON_OPERATION_STATUS = "mdi:heart-pulse"
ICON_INFORMATION = "mdi:information"

TEXT_SENSORS = [
    CONF_OPERATION_STATUS,
    CONF_ERRORS,
    CONF_BATTERY_TYPE,
    CONF_PASSWORD,
    CONF_DEVICE_TYPE,
    CONF_SOFTWARE_VERSION,
    CONF_MANUFACTURER,
    CONF_TOTAL_RUNTIME_FORMATTED,
    CONF_NETWORK_NODES_AVAILABLE,

    CONF_INFO_VENDORID,
    CONF_INFO_HARDWARE_VERSION,
    CONF_INFO_SOFTWARE_VERSION,
    CONF_INFO_DEVICE_NAME,
    CONF_INFO_DEVICE_PASSWORD,
    CONF_INFO_DEVICE_SERIAL_NUMBER,   
    CONF_INFO_DEVICE_SETUP_PASSCODE, 
]

CONFIG_SCHEMA = JK_RS485_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_MODE): cv.icon,
            }
        ),
        cv.Optional(CONF_ERRORS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_ERRORS): cv.icon,
            }
        ),
        cv.Optional(CONF_BATTERY_TYPE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_BATTERY_TYPE): cv.icon,
            }
        ),
        cv.Optional(CONF_PASSWORD): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_PASSWORD): cv.icon,
            }
        ),
        cv.Optional(CONF_DEVICE_TYPE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_SOFTWARE_VERSION): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_MANUFACTURER): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_TOTAL_RUNTIME_FORMATTED): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_TIMELAPSE): cv.icon,
            }
        ),
        cv.Optional(CONF_NETWORK_NODES_AVAILABLE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_NETWORK): cv.icon,
            }
        ),    
        cv.Optional(
            CONF_INFO_VENDORID
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_HARDWARE_VERSION
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_SOFTWARE_VERSION
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_DEVICE_NAME
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_DEVICE_PASSWORD
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),   
        cv.Optional(
            CONF_INFO_DEVICE_SERIAL_NUMBER
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),                    
        cv.Optional(
            CONF_INFO_DEVICE_SETUP_PASSCODE
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),   

        
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_RS485_BMS_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
