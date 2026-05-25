#!/usr/bin/env python3
"""Migrate JK BMS ESPHome YAML config keys from v2 to v3 naming."""

import re
import sys

SENSOR_RENAMES = {
    "alarm_low_volume": "low_soc_alarm_threshold",
    "balance_opening_pressure_difference": "balancing_delta_voltage",
    "balance_starting_voltage": "balancing_start_voltage",
    "balancer_status": "balancer_status_bitmask",
    "balancing": "balancer_status_bitmask",
    "battery_strings": "cell_count",
    "capacity_remaining_derived": "capacity_remaining",
    "capacity_remaining": "state_of_charge",
    "cell_pressure_difference_protection": "cell_voltage_difference_protection",
    "charging_high_temperature_protection": "charging_overtemperature_protection",
    "charging_low_temperature_protection": "charging_undertemperature_protection",
    "charging_low_temperature_recovery": "charging_undertemperature_recovery",
    "detail_log_count": "detail_log_entry_count",
    "discharging_high_temperature_protection": "discharging_overtemperature_protection",
    "discharging_low_temperature_protection": "discharging_undertemperature_protection",
    "discharging_low_temperature_recovery": "discharging_undertemperature_recovery",
    "low_soc_alarm": "low_soc_alarm_threshold",
    "mosfet_temperature_protection": "mosfet_overtemperature_protection",
    "mosfet_temperature_recovery": "mosfet_overtemperature_recovery",
    "power_tube_temperature": "mosfet_temperature",
    "power_tube_temperature_protection": "mosfet_overtemperature_protection",
    "power_tube_temperature_recovery": "mosfet_overtemperature_recovery",
    "temperature_sensor_temperature_difference_protection": "battery_temperature_difference_protection",
    "temperature_sensor_temperature_protection": "battery_overtemperature_protection",
    "temperature_sensor_temperature_recovery": "battery_overtemperature_recovery",
    "temperature_sensors": "temperature_sensor_count",
    "total_battery_capacity_setting": "full_charge_capacity",
}

TEXT_SENSOR_RENAMES = {
    "operation_status": "balancer_status",
}

BINARY_SENSOR_REMOVED = {
    "charging_switch": "use switch.charging instead",
    "discharging_switch": "use switch.discharging instead",
}

SECTION_KEYS = {"sensor", "binary_sensor", "text_sensor"}


def migrate(path):
    with open(path) as f:
        lines = f.readlines()

    section = None
    out = []
    changes = 0

    for lineno, line in enumerate(lines, 1):
        stripped = line.lstrip()
        indent = len(line) - len(stripped)

        # top-level section detection (no leading spaces before key:)
        top_match = re.match(r"^([a-z_]+)\s*:", line)
        if top_match and top_match.group(1) in SECTION_KEYS:
            section = top_match.group(1)
        elif top_match:
            section = None

        # key match: leading whitespace + exact key + colon
        key_match = re.match(r"^(\s+)([a-z_]+)(\s*:)", line)
        if key_match and indent > 0:
            key = key_match.group(2)

            if section == "sensor" and key in SENSOR_RENAMES:
                new_key = SENSOR_RENAMES[key]
                line = line[: key_match.start(2)] + new_key + line[key_match.end(2) :]
                print(f"  line {lineno}: {key} -> {new_key}")
                changes += 1

            elif section == "text_sensor" and key in TEXT_SENSOR_RENAMES:
                new_key = TEXT_SENSOR_RENAMES[key]
                line = line[: key_match.start(2)] + new_key + line[key_match.end(2) :]
                print(f"  line {lineno}: {key} -> {new_key}")
                changes += 1

            elif section == "binary_sensor" and key in BINARY_SENSOR_REMOVED:
                print(
                    f"  line {lineno}: WARNING: {key} removed — {BINARY_SENSOR_REMOVED[key]}"
                )

        out.append(line)

    with open(path, "w") as f:
        f.writelines(out)

    print(f"\n{changes} key(s) renamed in {path}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <config.yaml>")
        sys.exit(1)
    migrate(sys.argv[1])
