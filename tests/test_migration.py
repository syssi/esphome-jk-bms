"""Test migration on temporary files without ESPHome or attached devices."""

import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / "migrate_v2_to_v3.py"
spec = importlib.util.spec_from_file_location("migration", SCRIPT)
migration = importlib.util.module_from_spec(spec)
spec.loader.exec_module(migration)


class MigrationTest(unittest.TestCase):
    def migrate(self, text):
        with tempfile.TemporaryDirectory() as root:
            path = Path(root) / "config.yaml"
            path.write_text(text)
            with contextlib.redirect_stdout(io.StringIO()):
                migration.migrate(path)
            self.assertEqual(path.with_suffix(".yaml.bak").read_text(), text)
            return path.read_text()

    def test_hybrid_keeps_ble_capacity_in_amp_hours(self):
        text = """jk_bms:
jk_bms_ble:
sensor:
  - platform: jk_bms
    capacity_remaining:
      name: UART SOC
  - platform: jk_bms_ble
    capacity_remaining:
      name: BLE capacity
"""
        result = self.migrate(text)
        self.assertIn("    state_of_charge:\n      name: UART SOC", result)
        self.assertIn("    capacity_remaining:\n      name: BLE capacity", result)

    def test_other_platform_is_unchanged(self):
        text = """jk_bms:
sensor:
  - platform: unrelated
    capacity_remaining:
      name: unchanged
"""
        self.assertEqual(self.migrate(text), text)

    def test_nested_keys_are_unchanged(self):
        text = """jk_bms:
sensor:
  - platform: jk_bms
    capacity_remaining:
      capacity_remaining: leave metadata alone
"""
        result = self.migrate(text)
        self.assertIn("    state_of_charge:", result)
        self.assertIn("      capacity_remaining: leave metadata alone", result)

    def test_platform_can_follow_id(self):
        text = """sensor:
  - id: bms_sensor
    platform: jk_bms
    capacity_remaining:
      name: SOC
"""
        self.assertIn("    state_of_charge:", self.migrate(text))

    def test_unindented_list_and_quoted_platform(self):
        text = """sensor:
- platform: 'jk_bms_ble'
  detail_log_count:
    name: log count
"""
        self.assertIn("  detail_log_entry_count:", self.migrate(text))

    def test_other_sections_and_entities_are_unchanged(self):
        text = """jk_bms_ble:
number:
  - platform: unrelated
    power_tube_overtemperature_protection:
      name: unchanged
text_sensor:
  - platform: unrelated
    operation_status:
      name: unchanged
"""
        self.assertEqual(self.migrate(text), text)


if __name__ == "__main__":
    unittest.main()
