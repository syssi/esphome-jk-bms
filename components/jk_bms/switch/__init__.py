import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ID

from .. import CONF_JK_BMS_ID, JK_BMS_COMPONENT_SCHEMA, jk_bms_ns
from ..const import CONF_BALANCER, CONF_CHARGING, CONF_DISCHARGING

DEPENDENCIES = ["jk_bms"]

CODEOWNERS = ["@syssi"]

ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"
ICON_BALANCER = "mdi:seesaw"

SWITCHES = {
    CONF_CHARGING: 0xAB,
    CONF_DISCHARGING: 0xAC,
    # The BMS (v11) doesn't accept updates of register 0x9D at the moment
    CONF_BALANCER: 0x9D,
}

JkSwitch = jk_bms_ns.class_("JkSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_CHARGING,
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISCHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISCHARGING,
        ).extend(cv.COMPONENT_SCHEMA),
        # cv.Optional(CONF_BALANCER): switch.switch_schema(
        #     JkSwitch,
        #     icon=ICON_BALANCER,
        # ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
