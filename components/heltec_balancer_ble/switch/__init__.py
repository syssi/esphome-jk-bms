import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import (
    CONF_HELTEC_BALANCER_BLE_ID,
    HELTEC_BALANCER_BLE_COMPONENT_SCHEMA,
    heltec_balancer_ble_ns,
)

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCER = "balancer"

ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"
ICON_BALANCER = "mdi:seesaw"

SWITCHES = {
    CONF_BALANCER: 0x0D,
}

HeltecSwitch = heltec_balancer_ble_ns.class_(
    "HeltecSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCER): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(HeltecSwitch),
                cv.Optional(CONF_ICON, default=ICON_BALANCER): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
