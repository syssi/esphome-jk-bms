"""Deprecated aliases must not silently discard an explicit entity config."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).parent.parent))

import pytest  # noqa: E402
import voluptuous as vol  # noqa: E402

from components import jk_bms, jk_bms_ble  # noqa: E402
from components.jk_bms_ble import sensor as ble_sensor  # noqa: E402


@pytest.mark.parametrize("hub", [jk_bms, jk_bms_ble])
def test_old_and_new_names_are_rejected(hub):
    validate = hub.deprecated_renames({"old_name": "new_name"})
    config = {"old_name": {"name": "Old entity"}, "new_name": {"name": "New entity"}}
    with pytest.raises(vol.Invalid, match="old_name.*new_name"):
        validate(config)
    assert config["new_name"] == {"name": "New entity"}
    assert config["old_name"] == {"name": "Old entity"}


@pytest.mark.parametrize("hub", [jk_bms, jk_bms_ble])
def test_old_name_alone_still_migrates(hub):
    validate = hub.deprecated_renames({"old_name": "new_name"})
    config = {"old_name": {"name": "Existing entity"}}
    assert validate(config) == {"new_name": {"name": "Existing entity"}}
    assert "old_name" in config


@pytest.mark.parametrize("hub", [jk_bms, jk_bms_ble])
def test_new_name_alone_is_unchanged(hub):
    validate = hub.deprecated_renames({"old_name": "new_name"})
    config = {"new_name": {"name": "Existing entity"}}
    assert validate(config) == config


def test_two_ble_aliases_cannot_overwrite_each_other():
    with pytest.raises(vol.Invalid, match="balancer_status.*balancer_status_bitmask"):
        ble_sensor.CONFIG_SCHEMA(
            {
                "balancing": {"name": "First entity"},
                "balancer_status": {"name": "Second entity"},
            }
        )


def test_ble_sensor_schema_rejects_conflicting_temperature_entities():
    with pytest.raises(vol.Invalid, match="power_tube_temperature.*mosfet_temperature"):
        ble_sensor.CONFIG_SCHEMA(
            {
                "power_tube_temperature": {"name": "Old entity"},
                "mosfet_temperature": {"name": "New entity"},
            }
        )
