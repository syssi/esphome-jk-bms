import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_BMS_BLE_ID, JkBmsBle, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"

ICON_RETRIEVE_SETTINGS = "mdi:cog"

BUTTONS = {
    CONF_RETRIEVE_SETTINGS: 0x97,
}

JkButton = jk_bms_ble_ns.class_("JkButton", button.Button, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_BLE_ID): cv.use_id(JkBmsBle),
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_SETTINGS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            yield cg.register_component(var, conf)
            yield button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
