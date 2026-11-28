"""Entities the protocol version of their hub doesn't support are rejected at config time."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).parent.parent))

import esphome.config_validation as cv  # noqa: E402
from esphome.const import CONF_ID  # noqa: E402
import esphome.final_validate as fv  # noqa: E402
import pytest  # noqa: E402

from components.jk_bms_ble import (  # noqa: E402
    CONF_JK_BMS_BLE_ID,
    CONF_PROTOCOL_VERSION,
    PROTOCOL_VERSIONS,
    button,
    number,
    protocol_versions_from_registers,
    select,
    switch,
    text,
    validate_protocol_support,
)

HUB_ID = "bms0"


def _final_validate(validator, protocol_version, config, hub_id=HUB_ID):
    token = fv.full_config.set(
        {"jk_bms_ble": [{CONF_ID: hub_id, CONF_PROTOCOL_VERSION: protocol_version}]}
    )
    try:
        return validator({CONF_JK_BMS_BLE_ID: HUB_ID, **config})
    finally:
        fv.full_config.reset(token)


def _schema_keys(schema):
    return {
        str(key)
        for key in schema.schema
        if str(key) not in (CONF_ID, CONF_JK_BMS_BLE_ID)
    }


# Each platform, its schema and its protocol support map: every configurable entity must have an entry
PLATFORMS = [
    ("switch", switch.CONFIG_SCHEMA, switch.FINAL_VALIDATE_SCHEMA.supported),
    ("number", number._NUMBER_CONFIG_SCHEMA, number.FINAL_VALIDATE_SCHEMA.supported),
    ("select", select.CONFIG_SCHEMA, select.FINAL_VALIDATE_SCHEMA.supported),
    ("text", text.CONFIG_SCHEMA, text.FINAL_VALIDATE_SCHEMA.supported),
    ("button", button.CONFIG_SCHEMA, button.BUTTON_PROTOCOL_VERSIONS),
]


@pytest.mark.parametrize(("platform", "schema", "supported"), PLATFORMS)
def test_every_entity_has_protocol_versions(platform, schema, supported):
    assert _schema_keys(schema) == set(supported), platform


def test_protocol_versions_from_registers():
    table = {
        "a": [0x00, 0x1D, 0x1D, 0],
        "b": [0x6C, 0x00, 0x00, 0],
        "c": [0, 0, 0x27, 0],
    }
    assert protocol_versions_from_registers(table) == {
        "a": ("JK02_24S", "JK02_32S"),
        "b": ("JK04",),
        "c": ("JK02_32S",),
    }


def test_switch_and_number_follow_their_registers():
    supported = switch.FINAL_VALIDATE_SCHEMA.supported
    assert supported[switch.CONF_BALANCER] == PROTOCOL_VERSIONS
    assert supported[switch.CONF_CHARGING] == ("JK02_24S", "JK02_32S")
    assert supported[switch.CONF_HEATING] == ("JK02_32S",)


@pytest.mark.parametrize("protocol_version", ["JK04", "JK02_24S"])
def test_jk02_32s_switch_is_rejected(protocol_version):
    with pytest.raises(cv.Invalid, match="isn't supported by protocol_version"):
        _final_validate(
            switch.FINAL_VALIDATE_SCHEMA, protocol_version, {switch.CONF_HEATING: {}}
        )


def test_jk02_32s_switch_is_accepted():
    _final_validate(switch.FINAL_VALIDATE_SCHEMA, "JK02_32S", {switch.CONF_HEATING: {}})


def test_number_without_jk04_register_is_rejected_on_jk04():
    with pytest.raises(cv.Invalid, match="JK04"):
        _final_validate(
            number.FINAL_VALIDATE_SCHEMA,
            "JK04",
            {number.CONF_SMART_SLEEP_VOLTAGE: {}},
        )


def test_selects_are_jk02_32s_only():
    with pytest.raises(cv.Invalid, match="supported: JK02_32S"):
        _final_validate(
            select.FINAL_VALIDATE_SCHEMA, "JK02_24S", {select.CONF_UART1_PROTOCOL: {}}
        )
    _final_validate(
        select.FINAL_VALIDATE_SCHEMA, "JK02_32S", {select.CONF_UART1_PROTOCOL: {}}
    )


def test_texts_are_rejected_on_jk04():
    with pytest.raises(cv.Invalid, match="text 'device_password'"):
        _final_validate(
            text.FINAL_VALIDATE_SCHEMA, "JK04", {text.CONF_DEVICE_PASSWORD: {}}
        )
    _final_validate(
        text.FINAL_VALIDATE_SCHEMA, "JK02_24S", {text.CONF_DEVICE_PASSWORD: {}}
    )


def test_user_data_works_with_jk02():
    for protocol_version in ("JK02_24S", "JK02_32S"):
        _final_validate(
            text.FINAL_VALIDATE_SCHEMA, protocol_version, {text.CONF_USER_DATA: {}}
        )
    with pytest.raises(cv.Invalid, match="text 'user_data'"):
        _final_validate(text.FINAL_VALIDATE_SCHEMA, "JK04", {text.CONF_USER_DATA: {}})


@pytest.mark.parametrize("protocol_version", ["JK04", "JK02_24S"])
def test_user_data_2_is_jk02_32s_only(protocol_version):
    with pytest.raises(cv.Invalid, match="text 'user_data_2'.*supported: JK02_32S"):
        _final_validate(
            text.FINAL_VALIDATE_SCHEMA, protocol_version, {text.CONF_USER_DATA_2: {}}
        )


def test_user_data_2_on_jk02_32s_is_accepted():
    _final_validate(text.FINAL_VALIDATE_SCHEMA, "JK02_32S", {text.CONF_USER_DATA_2: {}})


def test_requests_work_with_every_protocol_version():
    for protocol_version in PROTOCOL_VERSIONS:
        _final_validate(
            button.FINAL_VALIDATE_SCHEMA,
            protocol_version,
            {button.CONF_RETRIEVE_SETTINGS: {}, button.CONF_RETRIEVE_DEVICE_INFO: {}},
        )


@pytest.mark.parametrize(
    ("key", "protocol_version"),
    [
        (button.CONF_SHUTDOWN, "JK04"),
        (button.CONF_RETRIEVE_LOGBOOK, "JK04"),
        (button.CONF_HEATING_TEST, "JK02_24S"),
    ],
)
def test_jk02_buttons_are_rejected(key, protocol_version):
    with pytest.raises(cv.Invalid, match=f"button '{key}'"):
        _final_validate(button.FINAL_VALIDATE_SCHEMA, protocol_version, {key: {}})


def test_other_hubs_are_ignored():
    _final_validate(
        switch.FINAL_VALIDATE_SCHEMA,
        "JK04",
        {switch.CONF_HEATING: {}},
        hub_id="other",
    )


@pytest.mark.parametrize(
    "supported", [{"a": ()}, {"a": ("JK05",)}], ids=["empty", "unknown"]
)
def test_invalid_support_maps_fail_on_import(supported):
    with pytest.raises(ValueError):
        validate_protocol_support("switch", supported)
