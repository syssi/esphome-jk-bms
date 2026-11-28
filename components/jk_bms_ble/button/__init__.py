import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    CONF_FACTORY_RESET,
    CONF_ID,
    CONF_RESTART,
    CONF_TIME_ID,
    ENTITY_CATEGORY_CONFIG,
    ICON_RESTART,
)
import esphome.final_validate as fv

from .. import (
    CONF_JK_BMS_BLE_ID,
    JK02_32S_ONLY,
    JK02_PROTOCOL_VERSIONS,
    JK_BMS_BLE_COMPONENT_SCHEMA,
    PROTOCOL_VERSIONS,
    VERIFY_NONE,
    check_write_table,
    jk_bms_ble_ns,
    validate_protocol_support,
)

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_DEVICE_INFO = "retrieve_device_info"
CONF_RETRIEVE_LOGBOOK = "retrieve_logbook"
CONF_SHUTDOWN = "shutdown"
CONF_HEATING_TEST = "heating_test"
CONF_SYNC_TIME = "sync_time"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RETRIEVE_DEVICE_INFO = "mdi:information-variant"
ICON_RETRIEVE_LOGBOOK = "mdi:history"
ICON_SHUTDOWN = "mdi:power"
ICON_FACTORY_RESET = "mdi:restore"
ICON_HEATING_TEST = "mdi:heat-wave"
ICON_SYNC_TIME = "mdi:clock-outline"

BUTTONS = {
    # register, verify
    # The BMS responds to the 0x96 request with a single settings frame and enables the notification stream
    CONF_RETRIEVE_SETTINGS: (0x96, VERIFY_NONE),
    CONF_RETRIEVE_DEVICE_INFO: (0x97, VERIFY_NONE),
    CONF_RETRIEVE_LOGBOOK: (0xA1, VERIFY_NONE),
    # JK02_32S: data_len=0 commands. Actions without a setting to read back.
    # The restart and the factory reset (which restarts the BMS) drop the connection.
    CONF_SHUTDOWN: (0x65, VERIFY_NONE),
    CONF_RESTART: (0x66, VERIFY_NONE),
    CONF_FACTORY_RESET: (0x9D, VERIFY_NONE),
    # Triggers the heating test (firmware >= V19.36). The BMS switches the heater off after 10 seconds.
    CONF_HEATING_TEST: (0x6D, VERIFY_NONE),
}
check_write_table(BUTTONS, ("register", "verify"))

JkButton = jk_bms_ble_ns.class_("JkButton", button.Button)
JkSyncTimeButton = jk_bms_ble_ns.class_("JkSyncTimeButton", button.Button)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_SETTINGS,
        ),
        cv.Optional(CONF_RETRIEVE_DEVICE_INFO): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_DEVICE_INFO,
        ),
        cv.Optional(CONF_RETRIEVE_LOGBOOK): button.button_schema(
            JkButton,
            icon=ICON_RETRIEVE_LOGBOOK,
        ),
        cv.Optional(CONF_SHUTDOWN): button.button_schema(
            JkButton,
            icon=ICON_SHUTDOWN,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_RESTART): button.button_schema(
            JkButton,
            icon=ICON_RESTART,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_FACTORY_RESET): button.button_schema(
            JkButton,
            icon=ICON_FACTORY_RESET,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_HEATING_TEST): button.button_schema(
            JkButton,
            icon=ICON_HEATING_TEST,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_SYNC_TIME): button.button_schema(
            JkSyncTimeButton,
            icon=ICON_SYNC_TIME,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


# The requests work with every protocol version. The other commands are JK02 commands: JK04 devices use their own
# register map (e.g. 0x6C is the balancer switch there). The heating test needs the heater of JK02_32S devices, see the
# heating switch.
BUTTON_PROTOCOL_VERSIONS = {
    CONF_RETRIEVE_SETTINGS: PROTOCOL_VERSIONS,
    CONF_RETRIEVE_DEVICE_INFO: PROTOCOL_VERSIONS,
    CONF_RETRIEVE_LOGBOOK: JK02_PROTOCOL_VERSIONS,
    CONF_SHUTDOWN: JK02_PROTOCOL_VERSIONS,
    CONF_RESTART: JK02_PROTOCOL_VERSIONS,
    CONF_FACTORY_RESET: JK02_PROTOCOL_VERSIONS,
    CONF_HEATING_TEST: JK02_32S_ONLY,
    CONF_SYNC_TIME: JK02_PROTOCOL_VERSIONS,
}
_validate_protocol_support = validate_protocol_support(
    "button", BUTTON_PROTOCOL_VERSIONS
)


def _final_validate(config):
    _validate_protocol_support(config)
    if CONF_SYNC_TIME not in config:
        return config

    hub_id = config[CONF_JK_BMS_BLE_ID]
    for hub in fv.full_config.get().get("jk_bms_ble", []):
        if hub[CONF_ID] == hub_id and CONF_TIME_ID not in hub:
            raise cv.Invalid(
                f"The '{CONF_SYNC_TIME}' button requires 'time_id' to be set on jk_bms_ble '{hub_id}'",
                path=[CONF_SYNC_TIME],
            )

    return config


FINAL_VALIDATE_SCHEMA = _final_validate


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, (address, verify) in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = await button.new_button(conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
            cg.add(var.set_verify(verify))

    if CONF_SYNC_TIME in config:
        conf = config[CONF_SYNC_TIME]
        var = await button.new_button(conf)
        cg.add(var.set_parent(hub))
