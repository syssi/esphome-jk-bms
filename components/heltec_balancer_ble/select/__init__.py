import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ID

from .. import (
    CONF_HELTEC_BALANCER_BLE_ID,
    HELTEC_BALANCER_BLE_COMPONENT_SCHEMA,
    heltec_balancer_ble_ns,
)

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_BUZZER_MODE = "buzzer_mode"
CONF_BATTERY_TYPE = "battery_type"

BUZZER_MODE_OPTIONS = ["Off", "Beep once", "Beep regular"]
BATTERY_TYPE_OPTIONS = ["NCM", "LFP", "LTO", "PbAc"]

SELECTS = {
    CONF_BUZZER_MODE: (0x14, BUZZER_MODE_OPTIONS),
    CONF_BATTERY_TYPE: (0x15, BATTERY_TYPE_OPTIONS),
}

HeltecSelect = heltec_balancer_ble_ns.class_(
    "HeltecSelect", select.Select, cg.Component
)

CONFIG_SCHEMA = HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BUZZER_MODE): select.select_schema(HeltecSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_BATTERY_TYPE): select.select_schema(HeltecSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key, (address, options) in SELECTS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await select.register_select(var, conf, options=options)
            cg.add(getattr(hub, f"set_{key}_select")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
