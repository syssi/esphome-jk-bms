from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from esphome.const import CONF_MAX_VALUE, CONF_MIN_VALUE
from esphome.core import CORE
import pytest

from components.jk_bms_ble import number

# NUMBERS entries: [JK04 register, JK02_24S register, JK02_32S register, factor, length, jk04_length]
FACTOR = 3
LENGTH = 4


@pytest.fixture(autouse=True)
def reset_core():
    CORE.reset()
    yield
    CORE.reset()


def _fits(raw: int, length: int) -> bool:
    bits = 8 * length
    if raw < 0:
        return raw >= -(1 << (bits - 1))
    return raw < (1 << bits)


@pytest.mark.parametrize("key", sorted(number.NUMBERS))
def test_default_range_fits_into_write_length(key):
    """The raw value of the default min/max must fit into the frame's data length.

    Otherwise the length byte of the write frame announces fewer bytes than the
    encoded value needs and the BMS stores a truncated value.
    """
    entry = number.NUMBERS[key]
    config = number.CONFIG_SCHEMA({key: {"name": key}})[key]

    for limit in (config[CONF_MIN_VALUE], config[CONF_MAX_VALUE]):
        raw = int(limit * entry[FACTOR])
        assert _fits(raw, entry[LENGTH]), (
            f"{key}: {limit} encodes to {raw}, which doesn't fit into "
            f"{entry[LENGTH]} byte(s)"
        )


def test_smart_sleep_voltage_is_written_as_uint32():
    # The smart sleep voltage is a 32-bit millivolt value (settings frame bytes 6-9).
    assert number.NUMBERS[number.CONF_SMART_SLEEP_VOLTAGE][LENGTH] == 4
