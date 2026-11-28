import esphome.codegen as cg
from esphome.components import text
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import (
    CONF_JK_BMS_BLE_ID,
    JK02_32S_ONLY,
    JK02_PROTOCOL_VERSIONS,
    JK_BMS_BLE_COMPONENT_SCHEMA,
    VERIFY_DEVICE_INFO,
    check_write_table,
    jk_bms_ble_ns,
    validate_protocol_support,
)

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_BLUETOOTH_NAME = "bluetooth_name"
CONF_DEVICE_PASSWORD = "device_password"
CONF_SETTINGS_PASSWORD = "settings_password"
CONF_USER_DATA = "user_data"
CONF_USER_DATA_2 = "user_data_2"

ICON_BLUETOOTH_NAME = "mdi:bluetooth"
ICON_DEVICE_PASSWORD = "mdi:key"
ICON_SETTINGS_PASSWORD = "mdi:form-textbox-password"
ICON_USER_DATA = "mdi:text-box-outline"

# The payload of a command frame holds 13 bytes
MAX_PAYLOAD_SIZE = 13
# The device info frame stores each text in 16 bytes, possibly empty. The entity traits allow that range, because Home
# Assistant rejects a state outside of them; writes are still limited to min_length and max_length.
STATE_MAX_LENGTH = 16

TEXTS = {
    # register, min_length, max_length, verify
    # The device name at device info bytes 46-61, which the BMS also advertises via Bluetooth. After a write the BMS
    # restarts its Bluetooth module, so the connection drops and the readback happens after the reconnect. Factory
    # names can be longer than 13 characters and can't be restored once changed.
    CONF_BLUETOOTH_NAME: [0x9B, 1, 13, VERIFY_DEVICE_INFO],
    # Stored at device info bytes 62-77, factory default 1234. The BMS only stores it: it is no Bluetooth pairing PIN and
    # doesn't protect the connection or writes.
    CONF_DEVICE_PASSWORD: [0x9C, 1, 13, VERIFY_DEVICE_INFO],
    # The app asks for this password before changing settings; the BMS doesn't check it. While it is the factory
    # default 123456 the BMS raises the "Modify password in time" alarm (red LED).
    CONF_SETTINGS_PASSWORD: [0xA0, 1, 13, VERIFY_DEVICE_INFO],
    # Free texts without any effect on the BMS, stored at device info bytes 102-117 and 134-149 (JK02_32S only).
    # Factory defaults are e.g. JK-BMS or "Input Userdata"; the latter has 14 characters and can't be restored once
    # changed.
    CONF_USER_DATA: [0x9F, 1, 13, VERIFY_DEVICE_INFO],
    CONF_USER_DATA_2: [0xA2, 1, 13, VERIFY_DEVICE_INFO],
}
check_write_table(TEXTS, ("register", "min_length", "max_length", "verify"))
for _key, (_, _min_length, _max_length, _) in TEXTS.items():
    if not 0 <= _min_length <= _max_length <= MAX_PAYLOAD_SIZE:
        raise ValueError(
            f"jk_bms_ble: text '{_key}' needs 0 <= min_length <= max_length <= {MAX_PAYLOAD_SIZE}"
        )

JkText = jk_bms_ble_ns.class_("JkText", text.Text, cg.Component)

# Texts restricted to the characters the BMS passes on to its Bluetooth module
NAME_CHARSET_TEXTS = (CONF_BLUETOOTH_NAME,)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BLUETOOTH_NAME): text.text_schema(
            JkText,
            icon=ICON_BLUETOOTH_NAME,
            entity_category=ENTITY_CATEGORY_CONFIG,
            mode="text",
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DEVICE_PASSWORD): text.text_schema(
            JkText,
            icon=ICON_DEVICE_PASSWORD,
            entity_category=ENTITY_CATEGORY_CONFIG,
            mode="text",
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_SETTINGS_PASSWORD): text.text_schema(
            JkText,
            icon=ICON_SETTINGS_PASSWORD,
            entity_category=ENTITY_CATEGORY_CONFIG,
            mode="text",
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_USER_DATA): text.text_schema(
            JkText,
            icon=ICON_USER_DATA,
            entity_category=ENTITY_CATEGORY_CONFIG,
            mode="text",
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_USER_DATA_2): text.text_schema(
            JkText,
            icon=ICON_USER_DATA,
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
        var = await text.new_text(conf, min_length=0, max_length=STATE_MAX_LENGTH)
        await cg.register_component(var, conf)
        cg.add(getattr(hub, f"set_{key}_text")(var))
        cg.add(var.set_parent(hub))
        cg.add(var.set_holding_register(address))
        cg.add(var.set_verify(verify))
        cg.add(var.set_min_write_length(min_length))
        cg.add(var.set_max_write_length(max_length))
        if key in NAME_CHARSET_TEXTS:
            cg.add(var.set_name_charset(True))


# JK04 devices have no text registers. User data 2 is stored in device info bytes only JK02_32S devices have.
FINAL_VALIDATE_SCHEMA = validate_protocol_support(
    "text",
    {
        **dict.fromkeys(TEXTS, JK02_PROTOCOL_VERSIONS),
        CONF_USER_DATA_2: JK02_32S_ONLY,
    },
)
