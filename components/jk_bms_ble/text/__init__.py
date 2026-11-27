import esphome.codegen as cg
from esphome.components import text
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import (
    CONF_JK_BMS_BLE_ID,
    JK_BMS_BLE_COMPONENT_SCHEMA,
    VERIFY_DEVICE_INFO,
    check_write_table,
    jk_bms_ble_ns,
)

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_SETTINGS_PASSWORD = "settings_password"

ICON_SETTINGS_PASSWORD = "mdi:form-textbox-password"

# The payload of a command frame holds 13 bytes
MAX_PAYLOAD_SIZE = 13

TEXTS = {
    # register, min_length, max_length, verify
    # The app asks for this password before changing settings; the BMS doesn't check it. While it is the factory
    # default 123456 the BMS raises the "Modify password in time" alarm (red LED).
    CONF_SETTINGS_PASSWORD: [0xA0, 1, 13, VERIFY_DEVICE_INFO],
}
check_write_table(TEXTS, ("register", "min_length", "max_length", "verify"))
for _key, (_, _min_length, _max_length, _) in TEXTS.items():
    if not 0 <= _min_length <= _max_length <= MAX_PAYLOAD_SIZE:
        raise ValueError(
            f"jk_bms_ble: text '{_key}' needs 0 <= min_length <= max_length <= {MAX_PAYLOAD_SIZE}"
        )

JkText = jk_bms_ble_ns.class_("JkText", text.Text, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SETTINGS_PASSWORD): text.text_schema(
            JkText,
            icon=ICON_SETTINGS_PASSWORD,
            entity_category=ENTITY_CATEGORY_CONFIG,
            mode="text",
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, (address, min_length, max_length, verify) in TEXTS.items():
        if key not in config:
            continue
        conf = config[key]
        var = await text.new_text(conf, min_length=min_length, max_length=max_length)
        await cg.register_component(var, conf)
        cg.add(getattr(hub, f"set_{key}_text")(var))
        cg.add(var.set_parent(hub))
        cg.add(var.set_holding_register(address))
        cg.add(var.set_verify(verify))
