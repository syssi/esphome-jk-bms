import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_BALANCER_ID, JK_BALANCER_COMPONENT_SCHEMA, jk_balancer_ns

DEPENDENCIES = ["jk_balancer"]

CODEOWNERS = ["@syssi"]

CONF_BALANCER = "balancer"

ICON_BALANCER = "mdi:seesaw"

SWITCHES = {
    CONF_BALANCER: 0xF6,
}

JkSwitch = jk_balancer_ns.class_("JkSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_BALANCER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCER): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkSwitch),
                cv.Optional(CONF_ICON, default=ICON_BALANCER): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BALANCER_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
