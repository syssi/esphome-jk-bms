import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_FACTORY_RESET, CONF_RESTART, ENTITY_CATEGORY_CONFIG

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_DEVICE_INFO = "retrieve_device_info"
CONF_RETRIEVE_LOGBOOK = "retrieve_logbook"
CONF_SHUTDOWN = "shutdown"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RETRIEVE_DEVICE_INFO = "mdi:information-variant"
ICON_RETRIEVE_LOGBOOK = "mdi:history"
ICON_SHUTDOWN = "mdi:power"
ICON_RESTART = "mdi:restart"
ICON_FACTORY_RESET = "mdi:restore"

BUTTONS = {
    # The BMS responds to the 0x96 request with a single settings frame and enables the notification stream
    CONF_RETRIEVE_SETTINGS: 0x96,
    CONF_RETRIEVE_DEVICE_INFO: 0x97,
    CONF_RETRIEVE_LOGBOOK: 0xA1,
    # JK02_32S: data_len=0 commands
    CONF_SHUTDOWN: 0x65,
    CONF_RESTART: 0x66,
    CONF_FACTORY_RESET: 0x9D,
}

JkButton = jk_bms_ble_ns.class_("JkButton", button.Button, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_SETTINGS,
        ),
        cv.Optional(CONF_RETRIEVE_DEVICE_INFO): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_DEVICE_INFO,
        ),
        cv.Optional(CONF_RETRIEVE_LOGBOOK): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_LOGBOOK,
        ),
        cv.Optional(CONF_SHUTDOWN): button.button_schema(
            JkButton,
            icon=ICON_SHUTDOWN,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_RESTART): button.button_schema(
            JkButton,
            icon=ICON_RESTART,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_FACTORY_RESET): button.button_schema(
            JkButton,
            icon=ICON_FACTORY_RESET,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = await button.new_button(conf)
            await cg.register_component(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
