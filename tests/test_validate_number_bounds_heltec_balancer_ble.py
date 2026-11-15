"""Heltec balancer number limits and steps must form valid number traits."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from esphome.core import CORE  # noqa: E402
import pytest  # noqa: E402
import voluptuous as vol  # noqa: E402

from components.heltec_balancer_ble import number  # noqa: E402

INVALID_OPTIONS = [
    {"min_value": 4, "max_value": 2},
    {"step": 0},
    {"step": -1},
    {"step": float("nan")},
    {"step": float("inf")},
    {"min_value": float("nan")},
    {"max_value": float("inf")},
]


@pytest.fixture(autouse=True)
def reset_core():
    CORE.reset()
    yield
    CORE.reset()


# balance_trigger_voltage is a legacy number, cell_overvoltage_protection a V2 one.
@pytest.mark.parametrize(
    "key", ["balance_trigger_voltage", "cell_overvoltage_protection"]
)
@pytest.mark.parametrize("options", INVALID_OPTIONS)
def test_invalid_number_traits_are_rejected(key, options):
    with pytest.raises(vol.Invalid):
        number.CONFIG_SCHEMA({key: {"name": "Limit", **options}})


def test_custom_limits_beyond_defaults_survive():
    config = number.CONFIG_SCHEMA(
        {
            "max_balance_current": {
                "name": "Current",
                "min_value": 0,
                "max_value": 50,
                "step": 0.5,
            }
        }
    )
    item = config["max_balance_current"]
    assert (item["min_value"], item["max_value"], item["step"]) == (0, 50, 0.5)
