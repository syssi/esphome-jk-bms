import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MODE,
    CONF_STEP,
    CONF_UNIT_OF_MEASUREMENT,
    ENTITY_CATEGORY_CONFIG,
    ICON_EMPTY,
    UNIT_EMPTY,
    UNIT_MILLIAMP,
)

from .. import CONF_JK_BALANCER_ID, JK_BALANCER_COMPONENT_SCHEMA, jk_balancer_ns

DEPENDENCIES = ["jk_balancer"]

CODEOWNERS = ["@syssi"]

UNIT_MILLIVOLT = "mV"

CONF_CELL_COUNT = "cell_count"
CONF_BALANCE_TRIGGER_VOLTAGE = "balance_trigger_voltage"
CONF_MAX_BALANCE_CURRENT = "max_balance_current"

NUMBERS = {
    CONF_CELL_COUNT: 0xF0,
    CONF_BALANCE_TRIGGER_VOLTAGE: 0xF2,
    CONF_MAX_BALANCE_CURRENT: 0xF4,
}

JkNumber = jk_balancer_ns.class_("JkNumber", number.Number, cg.Component)

JK_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(JkNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
        cv.Optional(CONF_STEP, default=1.0): cv.float_,
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES, upper=True),
        cv.Optional(
            CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
        ): cv.entity_category,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = JK_BALANCER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CELL_COUNT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=24): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_BALANCE_TRIGGER_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1000): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_MILLIVOLT
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_MAX_BALANCE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=2000): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_MILLIAMP
                ): cv.string_strict,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BALANCER_ID])
    for key, address in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await number.register_number(
                var,
                conf,
                min_value=conf[CONF_MIN_VALUE],
                max_value=conf[CONF_MAX_VALUE],
                step=conf[CONF_STEP],
            )
            cg.add(getattr(hub, f"set_{key}_number")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
