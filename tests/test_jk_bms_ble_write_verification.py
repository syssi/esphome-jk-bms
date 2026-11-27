"""Every writable entity must be verified with the frame that contains its value.

The verify flags live in the Python write tables. This test derives the expected
flags from the decoders in jk_bms_ble.cpp, so a new entity, or a value moved to
another frame, can't silently be verified with the wrong frame.
"""

from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import pytest

from components.jk_bms_ble import (
    VERIFY_DEVICE_INFO,
    VERIFY_NONE,
    VERIFY_SETTINGS,
    button,
    number,
    select,
    switch,
)

SOURCE = (
    Path(__file__).resolve().parents[1] / "components/jk_bms_ble/jk_bms_ble.cpp"
).read_text()


def _decoders_publishing(member: str) -> set[str]:
    """Names of the decode_*_ functions that publish the given entity member."""
    decoders = set()
    decoder = None
    for line in SOURCE.splitlines():
        if match := re.match(r"void JkBmsBle::(decode_\w+_)\(", line):
            decoder = match.group(1)
        elif decoder and f"this->{member}," in line and "publish_state_" in line:
            decoders.add(decoder)
    return decoders


def _expected(member: str) -> str:
    decoders = _decoders_publishing(member)
    if "decode_device_info_" in decoders:
        return str(VERIFY_DEVICE_INFO)
    if any(d.endswith("_settings_") for d in decoders):
        return str(VERIFY_SETTINGS)
    # Part of every cell info frame or not decoded at all: a readback shows nothing
    return str(VERIFY_NONE)


WRITE_TABLES = [
    *((key, f"{key}_switch_", entry[-1]) for key, entry in switch.SWITCHES.items()),
    *((key, f"{key}_number_", entry[-1]) for key, entry in number.NUMBERS.items()),
    *((key, f"{key}_select_", entry[-1]) for key, entry in select.SELECTS.items()),
]


@pytest.mark.parametrize(
    ("key", "member", "verify"), WRITE_TABLES, ids=[t[0] for t in WRITE_TABLES]
)
def test_entity_is_verified_with_the_frame_that_contains_its_value(key, member, verify):
    assert str(verify) == _expected(member), (
        f"{key} is published by {sorted(_decoders_publishing(member)) or 'no decoder'}"
    )


def test_read_requests_and_actions_are_not_verified():
    for key in (
        button.CONF_RETRIEVE_SETTINGS,
        button.CONF_RETRIEVE_DEVICE_INFO,
        button.CONF_RETRIEVE_LOGBOOK,
        button.CONF_SHUTDOWN,
        button.CONF_RESTART,
        button.CONF_FACTORY_RESET,
        button.CONF_HEATING_TEST,
    ):
        assert str(button.BUTTONS[key][1]) == str(VERIFY_NONE), key


def test_preset_is_verified_with_the_settings_frame():
    assert str(select.LOAD_CONFIG_PRESET_VERIFY) == str(VERIFY_SETTINGS)


def test_entry_without_verify_is_rejected():
    from components.jk_bms_ble import check_write_table

    with pytest.raises(ValueError, match="charging"):
        check_write_table(
            {"charging": [0x00, 0x1D, 0x1D]}, ("jk04", "jk02_24s", "jk02_32s", "verify")
        )
