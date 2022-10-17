import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_BALANCER = "balancer"

ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"
ICON_BALANCER = "mdi:seesaw"

SWITCHES = {
    CONF_CHARGING: [0x1D, 0x00],
    CONF_DISCHARGING: [0x1E, 0x00],
    CONF_BALANCER: [0x1F, 0x6C],
}

JkSwitch = jk_bms_ble_ns.class_("JkSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkSwitch),
                cv.Optional(CONF_ICON, default=ICON_CHARGING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISCHARGING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkSwitch),
                cv.Optional(CONF_ICON, default=ICON_DISCHARGING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_BALANCER): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkSwitch),
                cv.Optional(CONF_ICON, default=ICON_BALANCER): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_jk02_holding_register(address[0]))
            cg.add(var.set_jk04_holding_register(address[1]))
