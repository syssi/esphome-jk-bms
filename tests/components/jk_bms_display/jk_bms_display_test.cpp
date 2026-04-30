#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_display::testing {

// ── Voltage and current ──────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, TotalVoltage) {
  TestableJkBmsDisplay bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_NEAR(total.state, 26.61f, 0.001f);
}

TEST(JkBmsDisplayStatusTest, CurrentAndPower) {
  TestableJkBmsDisplay bms;
  sensor::Sensor current, power, charging_power, discharging_power;
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(current.state, 0.0f);
  EXPECT_FLOAT_EQ(power.state, 0.0f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_FLOAT_EQ(discharging_power.state, 0.0f);
}

// ── State of charge ──────────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, StateOfCharge) {
  TestableJkBmsDisplay bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(soc.state, 100.0f);
}

// ── Temperatures ─────────────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, Temperatures) {
  TestableJkBmsDisplay bms;
  sensor::Sensor mosfet_temp, battery_temp;
  bms.set_mosfet_temperature_sensor(&mosfet_temp);
  bms.set_battery_temperature_sensor(&battery_temp);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(mosfet_temp.state, 0.0f);
  EXPECT_FLOAT_EQ(battery_temp.state, 0.0f);
}

// ── Binary states ─────────────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, BinaryStates) {
  TestableJkBmsDisplay bms;
  binary_sensor::BinarySensor system_warning, balancing, charging, discharging;
  bms.set_system_warning_binary_sensor(&system_warning);
  bms.set_balancing_switch_binary_sensor(&balancing);
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_TRUE(system_warning.state);
  EXPECT_TRUE(balancing.state);
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, CellVoltages) {
  TestableJkBmsDisplay bms;
  sensor::Sensor cells[4];
  for (int i = 0; i < 4; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_NEAR(cells[0].state, 3.328f, 0.001f);
  EXPECT_NEAR(cells[1].state, 3.329f, 0.001f);
  EXPECT_NEAR(cells[2].state, 3.328f, 0.001f);
  EXPECT_NEAR(cells[3].state, 3.324f, 0.001f);
}

// ── Cell voltage statistics ───────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, CellVoltageStats) {
  TestableJkBmsDisplay bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_NEAR(min_v.state, 3.324f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.329f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 4.0f);  // cell 4 has minimum voltage
  EXPECT_FLOAT_EQ(max_cell.state, 2.0f);  // cell 2 has maximum voltage (first encountered)
  EXPECT_NEAR(delta.state, 0.005f, 0.001f);
  EXPECT_NEAR(avg.state, 3.327f, 0.001f);
}

// ── Alarm flags ───────────────────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, NoAlarms) {
  TestableJkBmsDisplay bms;
  binary_sensor::BinarySensor undervoltage, overvoltage, overcurrent, mosfet_ot, batt_ot;
  binary_sensor::BinarySensor short_circuit, coprocessor, balancer_wire, cell_mismatch;
  bms.set_cell_voltage_undervoltage_protection_binary_sensor(&undervoltage);
  bms.set_cell_voltage_overvoltage_protection_binary_sensor(&overvoltage);
  bms.set_overcurrent_protection_binary_sensor(&overcurrent);
  bms.set_mosfet_overtemperature_protection_binary_sensor(&mosfet_ot);
  bms.set_battery_temperature_protection_binary_sensor(&batt_ot);
  bms.set_short_circuit_protection_binary_sensor(&short_circuit);
  bms.set_coprocessor_communication_error_binary_sensor(&coprocessor);
  bms.set_balancer_wire_resistance_too_high_binary_sensor(&balancer_wire);
  bms.set_cell_count_mismatch_binary_sensor(&cell_mismatch);

  bms.on_jk_bms_display_data(STATUS_FRAME);

  EXPECT_FALSE(undervoltage.state);
  EXPECT_FALSE(overvoltage.state);
  EXPECT_FALSE(overcurrent.state);
  EXPECT_FALSE(mosfet_ot.state);
  EXPECT_FALSE(batt_ot.state);
  EXPECT_FALSE(short_circuit.state);
  EXPECT_FALSE(coprocessor.state);
  EXPECT_FALSE(balancer_wire.state);
  EXPECT_FALSE(cell_mismatch.state);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(JkBmsDisplayStatusTest, NullSensorsDoNotCrash) {
  TestableJkBmsDisplay bms;
  bms.on_jk_bms_display_data(STATUS_FRAME);
}

// ── Frame dispatch ────────────────────────────────────────────────────────────

TEST(JkBmsDisplayDispatchTest, TooShortFrameIgnored) {
  TestableJkBmsDisplay bms;
  bms.on_jk_bms_display_data({0xA5, 0x5A, 0x01, 0x82});
}

TEST(JkBmsDisplayDispatchTest, RawDataFrameIgnored) {
  TestableJkBmsDisplay bms;
  // frame_type=0x82, data_len=5 (> 4, ≠ 93) → routes to on_jk_bms_display_raw_data_
  bms.on_jk_bms_display_data({0xA5, 0x5A, 0x05, 0x82, 0x20, 0x53, 0x07, 0xE0});
}

TEST(JkBmsDisplayDispatchTest, Frame0x80Ignored) {
  TestableJkBmsDisplay bms;
  bms.on_jk_bms_display_data({0xA5, 0x5A, 0x03, 0x80, 0x01, 0x00});
}

TEST(JkBmsDisplayDispatchTest, UnknownFrameTypeIgnored) {
  TestableJkBmsDisplay bms;
  bms.on_jk_bms_display_data({0xA5, 0x5A, 0x03, 0xFF, 0x01, 0x00});
}

}  // namespace esphome::jk_bms_display::testing
