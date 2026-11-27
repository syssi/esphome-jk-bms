#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <memory>
#include "common.h"
#include "frames_jk04.h"

namespace esphome::jk_bms_ble::testing {

// ── Device info frame (frame type 0x03) ─────────────────────────────────────

TEST(JkBmsBleDeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(hw.state, "10.XG");
}

TEST(JkBmsBleDeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(sw.state, "10.07");
}

TEST(JkBmsBleDeviceInfoTest, DeviceModel) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor device_model;
  bms.set_device_model_text_sensor(&device_model);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(device_model.state, "JK-B2A24S20P");
}

TEST(JkBmsBleDeviceInfoTest, ManufacturingDate) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor manufacturing_date;
  bms.set_manufacturing_date_text_sensor(&manufacturing_date);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(manufacturing_date.state, "20220511");
}

TEST(JkBmsBleDeviceInfoTest, SerialNumber) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor serial_number;
  bms.set_serial_number_text_sensor(&serial_number);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(serial_number.state, "2041803028");
}

TEST(JkBmsBleDeviceInfoTest, PowerOnCount) {
  TestableJkBmsBle bms;
  sensor::Sensor power_on_count;
  bms.set_power_on_count_sensor(&power_on_count);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(power_on_count.state, 1.0f);
}

TEST(JkBmsBleDeviceInfoTest, NullSensorsDoNotCrash) {
  TestableJkBmsBle bms;
  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);
}

TEST(JkBmsBleDeviceInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);

  bms.decode_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(hw.state, "10.XG");
}

// ── JK02_24S cell info frame (frame type 0x02) ───────────────────────────────

TEST(JkBmsBleJk02CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  sensor::Sensor cells[24];
  for (int i = 0; i < 24; i++) {
    bms.set_cell_voltage_sensor(i, &cells[i]);
  }

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(cells[0].state, 3.284f, 0.001f);   // cell 1
  EXPECT_NEAR(cells[1].state, 3.280f, 0.001f);   // cell 2
  EXPECT_NEAR(cells[2].state, 3.279f, 0.001f);   // cell 3
  EXPECT_NEAR(cells[3].state, 3.280f, 0.001f);   // cell 4
  EXPECT_NEAR(cells[4].state, 3.280f, 0.001f);   // cell 5
  EXPECT_NEAR(cells[5].state, 3.279f, 0.001f);   // cell 6
  EXPECT_NEAR(cells[6].state, 3.279f, 0.001f);   // cell 7
  EXPECT_NEAR(cells[7].state, 3.281f, 0.001f);   // cell 8
  EXPECT_NEAR(cells[8].state, 3.280f, 0.001f);   // cell 9
  EXPECT_NEAR(cells[9].state, 3.279f, 0.001f);   // cell 10
  EXPECT_NEAR(cells[10].state, 3.280f, 0.001f);  // cell 11
  EXPECT_NEAR(cells[11].state, 3.279f, 0.001f);  // cell 12
  EXPECT_NEAR(cells[12].state, 3.282f, 0.001f);  // cell 13
  EXPECT_NEAR(cells[13].state, 3.284f, 0.001f);  // cell 14
  EXPECT_NEAR(cells[14].state, 3.279f, 0.001f);  // cell 15
  EXPECT_NEAR(cells[15].state, 3.279f, 0.001f);  // cell 16
  EXPECT_NEAR(cells[16].state, 3.280f, 0.001f);  // cell 17
  EXPECT_NEAR(cells[17].state, 3.279f, 0.001f);  // cell 18
  EXPECT_NEAR(cells[18].state, 3.281f, 0.001f);  // cell 19
  EXPECT_NEAR(cells[19].state, 3.282f, 0.001f);  // cell 20
  EXPECT_NEAR(cells[20].state, 3.282f, 0.001f);  // cell 21
  EXPECT_NEAR(cells[21].state, 3.282f, 0.001f);  // cell 22
  EXPECT_NEAR(cells[22].state, 3.285f, 0.001f);  // cell 23 (max)
  EXPECT_NEAR(cells[23].state, 3.281f, 0.001f);  // cell 24
}

TEST(JkBmsBleJk02CellInfoTest, CellVoltageStats) {
  TestableJkBmsBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(min_v.state, 3.279f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.285f, 0.001f);
  EXPECT_NEAR(min_cell.state, 3.0f, 0.1f);   // first min-voltage cell = cell 3
  EXPECT_NEAR(max_cell.state, 23.0f, 0.1f);  // first max-voltage cell = cell 23
  EXPECT_NEAR(delta.state, 0.006f, 0.001f);
  EXPECT_NEAR(avg.state, 3.281f, 0.001f);
}

TEST(JkBmsBleJk02CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsBleJk02CellInfoTest, ZeroCurrent) {
  TestableJkBmsBle bms;
  sensor::Sensor current, power, charging_power, discharging_power;
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(current.state, 0.0f);
  EXPECT_FLOAT_EQ(power.state, 0.0f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_FLOAT_EQ(discharging_power.state, 0.0f);
}

TEST(JkBmsBleJk02CellInfoTest, Temperatures) {
  TestableJkBmsBle bms;
  sensor::Sensor t1, t2, tube;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_mosfet_temperature_sensor(&tube);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(t1.state, 20.7f, 0.1f);
  EXPECT_NEAR(t2.state, 20.7f, 0.1f);
  EXPECT_NEAR(tube.state, 23.4f, 0.1f);
}

TEST(JkBmsBleJk02CellInfoTest, NoErrors) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor errors_bitmask_hex;
  text_sensor::TextSensor errors_text;
  bms.set_errors_bitmask_hex_text_sensor(&errors_bitmask_hex);
  bms.set_errors_text_sensor(&errors_text);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_EQ(errors_bitmask_hex.state, "0x00000000");
  EXPECT_EQ(errors_text.state, "");
}

TEST(JkBmsBleJk02CellInfoTest, BalancingOff) {
  TestableJkBmsBle bms;
  sensor::Sensor balancer_status_bitmask;
  binary_sensor::BinarySensor balancing_binary;
  bms.set_balancer_status_bitmask_sensor(&balancer_status_bitmask);
  bms.set_balancing_binary_sensor(&balancing_binary);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(balancer_status_bitmask.state, 0.0f);
  EXPECT_FALSE(balancing_binary.state);
}

TEST(JkBmsBleJk02CellInfoTest, StateOfCharge) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soc.state, 85.0f);
}

TEST(JkBmsBleJk02CellInfoTest, StateOfHealth) {
  TestableJkBmsBle bms;
  sensor::Sensor soh;
  bms.set_state_of_health_sensor(&soh);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soh.state, 100.0f);
}

TEST(JkBmsBleJk02CellInfoTest, Capacity) {
  TestableJkBmsBle bms;
  sensor::Sensor remaining, nominal, cycles;
  bms.set_capacity_remaining_sensor(&remaining);
  bms.set_full_charge_capacity_sensor(&nominal);
  bms.set_charging_cycles_sensor(&cycles);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(remaining.state, 42.804f, 0.001f);
  EXPECT_NEAR(nominal.state, 50.0f, 0.001f);
  EXPECT_FLOAT_EQ(cycles.state, 0.0f);
}

TEST(JkBmsBleJk02CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  sensor::Sensor runtime;
  text_sensor::TextSensor runtime_fmt;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_runtime_formatted_text_sensor(&runtime_fmt);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(runtime.state, 269340.0f);
  EXPECT_EQ(runtime_fmt.state, "3d 2h");
}

TEST(JkBmsBleJk02CellInfoTest, MosfetState) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor charging, discharging, precharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.set_precharging_binary_sensor(&precharging);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(JkBmsBleJk02CellInfoTest, HeatingOff) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor heating;
  bms.set_heating_binary_sensor(&heating);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FALSE(heating.state);
}

TEST(JkBmsBleJk02CellInfoTest, CellResistances) {
  TestableJkBmsBle bms;
  sensor::Sensor r1, r2;
  bms.set_cell_resistance_sensor(0, &r1);
  bms.set_cell_resistance_sensor(1, &r2);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(r1.state, 0.100f, 0.001f);  // cell 1 = 0x0064 = 100 mΩ
  EXPECT_NEAR(r2.state, 0.108f, 0.001f);  // cell 2 = 0x006C = 108 mΩ
}

TEST(JkBmsBleJk02CellInfoTest, NullSensorsDoNotCrash) {
  TestableJkBmsBle bms;
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
}

TEST(JkBmsBleJk02CellInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soc.state, 85.0f);
}

// ── Cell info throttling ─────────────────────────────────────────────────────

// A throttle interval longer than any possible uptime would drop every frame if the
// first one were compared against the zero-initialized timestamp.
static const uint32_t THROTTLE_FOREVER = std::numeric_limits<uint32_t>::max();

TEST(JkBmsBleThrottleTest, FirstJk02CellInfoIsNotThrottled) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.set_throttle(THROTTLE_FOREVER);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsBleThrottleTest, FirstJk04CellInfoIsNotThrottled) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.set_throttle(THROTTLE_FOREVER);

  bms.decode_jk04_cell_info_(CELL_INFO_JK04);

  EXPECT_NEAR(total.state, 53.691f, 0.001f);
}

TEST(JkBmsBleThrottleTest, SubsequentCellInfoWithinThrottleIsDropped) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.set_throttle(THROTTLE_FOREVER);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  total.state = NAN;

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(std::isnan(total.state));
}

// The throttle only rate-limits how often the cell info sensors get republished. It must not
// delay status_notification_received_, which stops update() from requesting cell info again.
TEST(JkBmsBleThrottleTest, Jk02ThrottledCellInfoStillMarksStatusNotificationReceived) {
  TestableJkBmsBle bms;
  bms.set_throttle(THROTTLE_FOREVER);
  bms.decode_(CELL_INFO_JK02_24S_V10);
  bms.status_notification_received_ = false;

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(bms.status_notification_received_);
}

TEST(JkBmsBleThrottleTest, Jk04ThrottledCellInfoStillMarksStatusNotificationReceived) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  bms.set_throttle(THROTTLE_FOREVER);
  bms.decode_(CELL_INFO_JK04);
  bms.status_notification_received_ = false;

  bms.decode_(CELL_INFO_JK04);

  EXPECT_TRUE(bms.status_notification_received_);
}

// ── Protocol version dispatch ────────────────────────────────────────────────

TEST(JkBmsBleDispatchTest, FrameType02DispatchesToCellInfo) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsBleDispatchTest, FrameType03DispatchesToDeviceInfo) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);

  bms.decode_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(sw.state, "10.07");
}

TEST(JkBmsBleDispatchTest, Jk04ModeDispatches) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);

  // JK04 cell info dispatch should not crash even with a JK02 frame body
  bms.decode_(CELL_INFO_JK02_24S_V10);
}

TEST(JkBmsBlePublishDeviceUnavailableTest, InvalidatesBinarySensors) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor online, balancing, precharging, charging, discharging, heating, dry_contact_1;
  binary_sensor::BinarySensor dry_contact_2;
  bms.set_online_status_binary_sensor(&online);
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_precharging_binary_sensor(&precharging);
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.set_heating_binary_sensor(&heating);
  bms.set_dry_contact_1_binary_sensor(&dry_contact_1);
  bms.set_dry_contact_2_binary_sensor(&dry_contact_2);
  const std::vector<binary_sensor::BinarySensor *> binary_sensors = {
      &balancing, &precharging, &charging, &discharging, &heating, &dry_contact_1, &dry_contact_2};

  online.publish_state(true);
  for (auto *binary_sensor : binary_sensors) {
    binary_sensor->publish_state(true);
  }

  bms.publish_device_unavailable_();

  EXPECT_TRUE(online.has_state());
  EXPECT_FALSE(online.state);
  for (size_t i = 0; i < binary_sensors.size(); i++) {
    EXPECT_FALSE(binary_sensors[i]->has_state()) << "binary sensor index " << i;
  }
}

// The following tests assign every entity of the component to an expected outcome of
// publish_device_unavailable_(): cleared (NAN / "Offline") or left untouched. Clearing an
// additional entity, or no longer clearing one, makes one of them fail. New entities must be
// added to the matching list.

template<typename T> struct NamedSetter {
  const char *name;
  void (JkBmsBle::*setter)(T *);
};

#define ENTITY_SETTER(name) {#name, &JkBmsBle::name}

template<typename Entity, typename T>
std::vector<std::unique_ptr<Entity>> attach_entities(JkBmsBle &bms, const std::vector<NamedSetter<T>> &setters) {
  std::vector<std::unique_ptr<Entity>> entities;
  for (const auto &entry : setters) {
    entities.push_back(std::make_unique<Entity>());
    (bms.*entry.setter)(entities.back().get());
  }
  return entities;
}

TEST(JkBmsBlePublishDeviceUnavailableTest, ClearsLiveSensorsAndKeepsStaticSensors) {
  const std::vector<NamedSetter<sensor::Sensor>> cleared_setters = {
      ENTITY_SETTER(set_balancer_status_bitmask_sensor),
      ENTITY_SETTER(set_min_cell_voltage_sensor),
      ENTITY_SETTER(set_max_cell_voltage_sensor),
      ENTITY_SETTER(set_min_voltage_cell_sensor),
      ENTITY_SETTER(set_max_voltage_cell_sensor),
      ENTITY_SETTER(set_delta_cell_voltage_sensor),
      ENTITY_SETTER(set_average_cell_voltage_sensor),
      ENTITY_SETTER(set_total_voltage_sensor),
      ENTITY_SETTER(set_current_sensor),
      ENTITY_SETTER(set_power_sensor),
      ENTITY_SETTER(set_charging_power_sensor),
      ENTITY_SETTER(set_discharging_power_sensor),
      ENTITY_SETTER(set_mosfet_temperature_sensor),
      ENTITY_SETTER(set_state_of_charge_sensor),
      ENTITY_SETTER(set_state_of_health_sensor),
      ENTITY_SETTER(set_capacity_remaining_sensor),
      ENTITY_SETTER(set_full_charge_capacity_sensor),
      ENTITY_SETTER(set_charging_cycles_sensor),
      ENTITY_SETTER(set_total_charging_cycle_capacity_sensor),
      ENTITY_SETTER(set_total_runtime_sensor),
      ENTITY_SETTER(set_balancing_current_sensor),
      ENTITY_SETTER(set_heating_current_sensor),
      ENTITY_SETTER(set_emergency_time_countdown_sensor),
      ENTITY_SETTER(set_smart_sleep_countdown_sensor),
      ENTITY_SETTER(set_discharge_overcurrent_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_discharge_short_circuit_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_charge_overcurrent_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_charge_short_circuit_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_cell_undervoltage_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_cell_overvoltage_protection_recovery_countdown_sensor),
      ENTITY_SETTER(set_charge_status_id_sensor),
      ENTITY_SETTER(set_charge_status_time_elapsed_sensor),
      ENTITY_SETTER(set_detail_log_entry_count_sensor),
  };
  const std::vector<NamedSetter<sensor::Sensor>> retained_setters = {
      ENTITY_SETTER(set_power_on_count_sensor),
      ENTITY_SETTER(set_battery_type_id_sensor),
      ENTITY_SETTER(set_device_address_sensor),
      ENTITY_SETTER(set_uart1_protocols_enabled_bitmask_sensor),
      ENTITY_SETTER(set_uart2_protocols_enabled_bitmask_sensor),
      ENTITY_SETTER(set_uart3_protocols_enabled_bitmask_sensor),
      ENTITY_SETTER(set_can_protocols_enabled_bitmask_sensor),
  };
  TestableJkBmsBle bms;
  auto cleared = attach_entities<sensor::Sensor>(bms, cleared_setters);
  auto retained = attach_entities<sensor::Sensor>(bms, retained_setters);
  sensor::Sensor cell_voltages[32], cell_resistances[32], temperatures[5];
  for (uint8_t i = 0; i < 32; i++) {
    bms.set_cell_voltage_sensor(i, &cell_voltages[i]);
    bms.set_cell_resistance_sensor(i, &cell_resistances[i]);
  }
  for (uint8_t i = 0; i < 5; i++) {
    bms.set_temperature_sensor(i, &temperatures[i]);
  }

  for (auto &sensor : cleared) {
    sensor->publish_state(42.0f);
  }
  for (auto &sensor : retained) {
    sensor->publish_state(42.0f);
  }
  for (uint8_t i = 0; i < 32; i++) {
    cell_voltages[i].publish_state(3.3f);
    cell_resistances[i].publish_state(0.1f);
  }
  for (auto &temperature : temperatures) {
    temperature.publish_state(25.0f);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < cleared.size(); i++) {
    EXPECT_TRUE(std::isnan(cleared[i]->state)) << cleared_setters[i].name;
  }
  for (size_t i = 0; i < retained.size(); i++) {
    EXPECT_FLOAT_EQ(retained[i]->state, 42.0f) << retained_setters[i].name;
  }
  for (uint8_t i = 0; i < 32; i++) {
    EXPECT_TRUE(std::isnan(cell_voltages[i].state)) << "cell voltage " << (int) i;
    EXPECT_TRUE(std::isnan(cell_resistances[i].state)) << "cell resistance " << (int) i;
  }
  for (uint8_t i = 0; i < 5; i++) {
    EXPECT_TRUE(std::isnan(temperatures[i].state)) << "temperature " << (int) i;
  }
}

TEST(JkBmsBlePublishDeviceUnavailableTest, MarksLiveTextSensorsOfflineAndKeepsStaticOnes) {
  const std::vector<NamedSetter<text_sensor::TextSensor>> offline_setters = {
      ENTITY_SETTER(set_errors_text_sensor),
      ENTITY_SETTER(set_balancer_status_text_sensor),
      ENTITY_SETTER(set_total_runtime_formatted_text_sensor),
      ENTITY_SETTER(set_charge_status_text_sensor),
      ENTITY_SETTER(set_errors_bitmask_hex_text_sensor),
      ENTITY_SETTER(set_device_time_text_sensor),
  };
  const std::vector<NamedSetter<text_sensor::TextSensor>> retained_setters = {
      ENTITY_SETTER(set_software_version_text_sensor), ENTITY_SETTER(set_hardware_version_text_sensor),
      ENTITY_SETTER(set_battery_type_text_sensor),     ENTITY_SETTER(set_device_model_text_sensor),
      ENTITY_SETTER(set_device_name_text_sensor),      ENTITY_SETTER(set_manufacturing_date_text_sensor),
      ENTITY_SETTER(set_serial_number_text_sensor),
  };
  TestableJkBmsBle bms;
  auto offline = attach_entities<text_sensor::TextSensor>(bms, offline_setters);
  auto retained = attach_entities<text_sensor::TextSensor>(bms, retained_setters);

  for (auto &text_sensor : offline) {
    text_sensor->publish_state("before");
  }
  for (auto &text_sensor : retained) {
    text_sensor->publish_state("before");
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < offline.size(); i++) {
    EXPECT_EQ(offline[i]->state, "Offline") << offline_setters[i].name;
  }
  for (size_t i = 0; i < retained.size(); i++) {
    EXPECT_EQ(retained[i]->state, "before") << retained_setters[i].name;
  }
}

TEST(JkBmsBlePublishDeviceUnavailableTest, KeepsNumbers) {
  const std::vector<NamedSetter<number::Number>> setters = {
      ENTITY_SETTER(set_smart_sleep_voltage_number),
      ENTITY_SETTER(set_balance_trigger_voltage_number),
      ENTITY_SETTER(set_cell_soc100_voltage_number),
      ENTITY_SETTER(set_cell_soc0_voltage_number),
      ENTITY_SETTER(set_cell_request_charge_voltage_number),
      ENTITY_SETTER(set_cell_request_float_voltage_number),
      ENTITY_SETTER(set_cell_request_charge_voltage_time_number),
      ENTITY_SETTER(set_cell_request_float_voltage_time_number),
      ENTITY_SETTER(set_cell_count_number),
      ENTITY_SETTER(set_total_battery_capacity_number),
      ENTITY_SETTER(set_cell_voltage_overvoltage_protection_number),
      ENTITY_SETTER(set_cell_voltage_overvoltage_recovery_number),
      ENTITY_SETTER(set_cell_voltage_undervoltage_protection_number),
      ENTITY_SETTER(set_cell_voltage_undervoltage_recovery_number),
      ENTITY_SETTER(set_balancing_start_voltage_number),
      ENTITY_SETTER(set_voltage_calibration_number),
      ENTITY_SETTER(set_current_calibration_number),
      ENTITY_SETTER(set_power_off_voltage_number),
      ENTITY_SETTER(set_max_balance_current_number),
      ENTITY_SETTER(set_max_charge_current_number),
      ENTITY_SETTER(set_max_discharge_current_number),
      ENTITY_SETTER(set_charge_overcurrent_protection_delay_number),
      ENTITY_SETTER(set_charge_overcurrent_protection_recovery_time_number),
      ENTITY_SETTER(set_discharge_overcurrent_protection_delay_number),
      ENTITY_SETTER(set_discharge_overcurrent_protection_recovery_time_number),
      ENTITY_SETTER(set_short_circuit_protection_delay_number),
      ENTITY_SETTER(set_short_circuit_protection_recovery_time_number),
      ENTITY_SETTER(set_charge_overtemperature_protection_number),
      ENTITY_SETTER(set_charge_overtemperature_protection_recovery_number),
      ENTITY_SETTER(set_discharge_overtemperature_protection_number),
      ENTITY_SETTER(set_discharge_overtemperature_protection_recovery_number),
      ENTITY_SETTER(set_charge_undertemperature_protection_number),
      ENTITY_SETTER(set_charge_undertemperature_protection_recovery_number),
      ENTITY_SETTER(set_discharge_undertemperature_protection_number),
      ENTITY_SETTER(set_discharge_undertemperature_protection_recovery_number),
      ENTITY_SETTER(set_mosfet_overtemperature_protection_number),
      ENTITY_SETTER(set_mosfet_overtemperature_protection_recovery_number),
      ENTITY_SETTER(set_discharge_precharge_time_number),
      ENTITY_SETTER(set_heating_start_temperature_number),
      ENTITY_SETTER(set_heating_stop_temperature_number),
      ENTITY_SETTER(set_smart_sleep_delay_number),
      ENTITY_SETTER(set_emergency_duration_number),
      ENTITY_SETTER(set_re_bulk_soc_number),
      ENTITY_SETTER(set_soc_calibration_number),
      ENTITY_SETTER(set_soh_calibration_number),
  };
  TestableJkBmsBle bms;
  auto numbers = attach_entities<TestNumber>(bms, setters);

  for (auto &number : numbers) {
    number->publish_state(42.0f);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < numbers.size(); i++) {
    EXPECT_FLOAT_EQ(numbers[i]->state, 42.0f) << setters[i].name;
  }
}

TEST(JkBmsBlePublishDeviceUnavailableTest, KeepsSwitches) {
  const std::vector<NamedSetter<switch_::Switch>> setters = {
      ENTITY_SETTER(set_charging_switch),
      ENTITY_SETTER(set_discharging_switch),
      ENTITY_SETTER(set_balancer_switch),
      ENTITY_SETTER(set_emergency_switch),
      ENTITY_SETTER(set_heating_switch),
      ENTITY_SETTER(set_disable_temperature_sensors_switch),
      ENTITY_SETTER(set_display_always_on_switch),
      ENTITY_SETTER(set_smart_sleep_switch),
      ENTITY_SETTER(set_timed_stored_data_switch),
      ENTITY_SETTER(set_disable_pcl_module_switch),
      ENTITY_SETTER(set_charging_float_mode_switch),
      ENTITY_SETTER(set_emergency_button_trigger_switch),
      ENTITY_SETTER(set_dry_contact_alarm_intermittent_switch),
      ENTITY_SETTER(set_discharge_overcurrent_protection_2_switch),
      ENTITY_SETTER(set_discharge_overcurrent_protection_3_switch),
      ENTITY_SETTER(set_gps_locked_charging_switch),
      ENTITY_SETTER(set_gps_locked_discharging_switch),
  };
  TestableJkBmsBle bms;
  auto switches = attach_entities<TestSwitch>(bms, setters);

  for (auto &switch_entity : switches) {
    switch_entity->publish_state(true);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < switches.size(); i++) {
    EXPECT_TRUE(switches[i]->state) << setters[i].name;
  }
}

TEST(JkBmsBlePublishDeviceUnavailableTest, KeepsSelects) {
  const std::vector<NamedSetter<select::Select>> setters = {
      ENTITY_SETTER(set_uart1_protocol_select), ENTITY_SETTER(set_uart2_protocol_select),
      ENTITY_SETTER(set_can_protocol_select),   ENTITY_SETTER(set_lcd_buzzer_trigger_select),
      ENTITY_SETTER(set_uart3_protocol_select), ENTITY_SETTER(set_dry1_trigger_select),
      ENTITY_SETTER(set_dry2_trigger_select),   ENTITY_SETTER(set_multiplexed_port_mode_select),
  };
  TestableJkBmsBle bms;
  std::vector<std::unique_ptr<TestSelect>> selects;
  for (const auto &entry : setters) {
    selects.push_back(std::make_unique<TestSelect>(std::initializer_list<const char *>{"A", "B"}));
    (bms.*entry.setter)(selects.back().get());
  }

  for (auto &select : selects) {
    select->publish_state(size_t{1});
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < selects.size(); i++) {
    EXPECT_EQ(selects[i]->active_index().value_or(0), 1u) << setters[i].name;
  }
}

#undef ENTITY_SETTER

}  // namespace esphome::jk_bms_ble::testing
