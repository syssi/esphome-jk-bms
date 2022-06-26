import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_DEVICE_INFO = "retrieve_device_info"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RETRIEVE_DEVICE_INFO = "mdi:information-variant"

BUTTONS = {
    # The BMS responds to the 0x96 request with a single settings frame and enables the notification stream
    CONF_RETRIEVE_SETTINGS: 0x96,
    CONF_RETRIEVE_DEVICE_INFO: 0x97,
}

JkButton = jk_bms_ble_ns.class_("JkButton", button.Button, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_SETTINGS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RETRIEVE_DEVICE_INFO): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_DEVICE_INFO): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
