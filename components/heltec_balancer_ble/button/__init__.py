import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import (
    CONF_HELTEC_BALANCER_BLE_ID,
    HELTEC_BALANCER_BLE_COMPONENT_SCHEMA,
    heltec_balancer_ble_ns,
)

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_DEVICE_INFO = "retrieve_device_info"
CONF_RETRIEVE_FACTORY_DEFAULTS = "retrieve_factory_defaults"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RETRIEVE_DEVICE_INFO = "mdi:information-variant"
ICON_RETRIEVE_FACTORY_DEFAULTS = "mdi:factory"

BUTTONS = {
    CONF_RETRIEVE_SETTINGS: 0x04,
    CONF_RETRIEVE_DEVICE_INFO: 0x01,
    CONF_RETRIEVE_FACTORY_DEFAULTS: 0x03,
}

HeltecButton = heltec_balancer_ble_ns.class_(
    "HeltecButton", button.Button, cg.Component
)

CONFIG_SCHEMA = HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(HeltecButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_SETTINGS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RETRIEVE_DEVICE_INFO): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(HeltecButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_DEVICE_INFO): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RETRIEVE_FACTORY_DEFAULTS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(HeltecButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_FACTORY_DEFAULTS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
