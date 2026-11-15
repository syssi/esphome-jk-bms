from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import importlib.util

import pytest


@pytest.mark.parametrize(
    "platform,old,new",
    [
        ("jk_bms", "capacity_remaining", "state_of_charge"),
        ("jk_bms_ble", "power_tube_temperature", "mosfet_temperature"),
    ],
)
def test_mapping_form_is_migrated(tmp_path, platform, old, new):
    path = tmp_path / "config.yaml"
    path.write_text(f"sensor:\n  platform: {platform}\n  {old}:\n    name: Sensor\n")
    script = Path(__file__).resolve().parents[1] / "migrate_v2_to_v3.py"
    spec = importlib.util.spec_from_file_location("migration_mapping", script)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.migrate(path)
    assert f"  {new}:" in path.read_text()
    assert f"  {old}:" not in path.read_text()
    assert path.with_suffix(".yaml.bak").is_file()
