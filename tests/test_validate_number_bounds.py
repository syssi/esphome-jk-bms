from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from esphome.core import CORE
import pytest
import voluptuous as vol

from components.jk_bms_ble import number


@pytest.fixture(autouse=True)
def reset_core():
    CORE.reset()
    yield
    CORE.reset()


@pytest.mark.parametrize(
    "options",
    [
        {"min_value": 4, "max_value": 2},
        {"step": 0},
        {"step": -1},
        {"step": float("nan")},
        {"step": float("inf")},
        {"min_value": float("nan")},
        {"max_value": float("inf")},
    ],
)
def test_invalid_number_traits_are_rejected(options):
    with pytest.raises(vol.Invalid):
        number.CONFIG_SCHEMA({"balance_trigger_voltage": {"name": "Delta", **options}})


def test_valid_custom_limits_survive():
    config = number.CONFIG_SCHEMA(
        {
            "balance_trigger_voltage": {
                "name": "Delta",
                "min_value": 0.01,
                "max_value": 0.1,
                "step": 0.001,
            }
        }
    )
    assert config["balance_trigger_voltage"]["step"] == 0.001
