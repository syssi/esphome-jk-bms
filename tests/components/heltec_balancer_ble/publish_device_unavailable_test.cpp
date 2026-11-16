#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include "common.h"

namespace esphome::heltec_balancer_ble::testing {

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, InvalidatesBinarySensors) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor online, balancing, error_charging, error_discharging, error_system_overheating;
  bms.set_online_status_binary_sensor(&online);
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_error_charging_binary_sensor(&error_charging);
  bms.set_error_discharging_binary_sensor(&error_discharging);
  bms.set_error_system_overheating_binary_sensor(&error_system_overheating);
  const std::vector<binary_sensor::BinarySensor *> binary_sensors = {&balancing, &error_charging, &error_discharging,
                                                                     &error_system_overheating};

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
  void (HeltecBalancerBle::*setter)(T *);
};

#define ENTITY_SETTER(name) {#name, &HeltecBalancerBle::name}

template<typename Entity, typename T>
std::vector<std::unique_ptr<Entity>> attach_entities(HeltecBalancerBle &bms,
                                                     const std::vector<NamedSetter<T>> &setters) {
  std::vector<std::unique_ptr<Entity>> entities;
  for (const auto &entry : setters) {
    entities.push_back(std::make_unique<Entity>());
    (bms.*entry.setter)(entities.back().get());
  }
  return entities;
}

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, ClearsSensors) {
  const std::vector<NamedSetter<sensor::Sensor>> cleared_setters = {
      ENTITY_SETTER(set_min_cell_voltage_sensor),
      ENTITY_SETTER(set_max_cell_voltage_sensor),
      ENTITY_SETTER(set_min_voltage_cell_sensor),
      ENTITY_SETTER(set_max_voltage_cell_sensor),
      ENTITY_SETTER(set_delta_cell_voltage_sensor),
      ENTITY_SETTER(set_average_cell_voltage_sensor),
      ENTITY_SETTER(set_total_voltage_sensor),
      ENTITY_SETTER(set_temperature_sensor_1_sensor),
      ENTITY_SETTER(set_temperature_sensor_2_sensor),
      ENTITY_SETTER(set_mosfet_temperature_sensor),
      ENTITY_SETTER(set_balancer_temperature_sensor),
      ENTITY_SETTER(set_nominal_capacity_sensor),
      ENTITY_SETTER(set_capacity_remaining_sensor),
      ENTITY_SETTER(set_state_of_charge_sensor),
      ENTITY_SETTER(set_total_runtime_sensor),
      ENTITY_SETTER(set_power_on_count_sensor),
      ENTITY_SETTER(set_balancing_current_sensor),
      ENTITY_SETTER(set_errors_bitmask_sensor),
      ENTITY_SETTER(set_cell_detection_failed_bitmask_sensor),
      ENTITY_SETTER(set_cell_overvoltage_bitmask_sensor),
      ENTITY_SETTER(set_cell_undervoltage_bitmask_sensor),
      ENTITY_SETTER(set_cell_polarity_error_bitmask_sensor),
      ENTITY_SETTER(set_cell_excessive_line_resistance_bitmask_sensor),
  };
  TestableHeltecBalancerBle bms;
  auto cleared = attach_entities<sensor::Sensor>(bms, cleared_setters);
  sensor::Sensor cell_voltages[24], cell_resistances[24];
  for (uint8_t i = 0; i < 24; i++) {
    bms.set_cell_voltage_sensor(i, &cell_voltages[i]);
    bms.set_cell_resistance_sensor(i, &cell_resistances[i]);
  }

  for (auto &sensor : cleared) {
    sensor->publish_state(42.0f);
  }
  for (uint8_t i = 0; i < 24; i++) {
    cell_voltages[i].publish_state(3.3f);
    cell_resistances[i].publish_state(0.1f);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < cleared.size(); i++) {
    EXPECT_TRUE(std::isnan(cleared[i]->state)) << cleared_setters[i].name;
  }
  for (uint8_t i = 0; i < 24; i++) {
    EXPECT_TRUE(std::isnan(cell_voltages[i].state)) << "cell voltage " << (int) i;
    EXPECT_TRUE(std::isnan(cell_resistances[i].state)) << "cell resistance " << (int) i;
  }
}

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, MarksLiveTextSensorsOfflineAndKeepsStaticOnes) {
  const std::vector<NamedSetter<text_sensor::TextSensor>> offline_setters = {
      ENTITY_SETTER(set_operation_status_text_sensor),
      ENTITY_SETTER(set_total_runtime_formatted_text_sensor),
  };
  const std::vector<NamedSetter<text_sensor::TextSensor>> retained_setters = {
      ENTITY_SETTER(set_errors_text_sensor),           ENTITY_SETTER(set_buzzer_mode_text_sensor),
      ENTITY_SETTER(set_battery_type_text_sensor),     ENTITY_SETTER(set_device_model_text_sensor),
      ENTITY_SETTER(set_hardware_version_text_sensor), ENTITY_SETTER(set_software_version_text_sensor),
      ENTITY_SETTER(set_protocol_version_text_sensor), ENTITY_SETTER(set_manufacturing_date_text_sensor),
  };
  TestableHeltecBalancerBle bms;
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

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, KeepsNumbers) {
  const std::vector<NamedSetter<number::Number>> setters = {
      ENTITY_SETTER(set_cell_count_number),
      ENTITY_SETTER(set_balance_trigger_voltage_number),
      ENTITY_SETTER(set_max_balance_current_number),
      ENTITY_SETTER(set_balance_sleep_voltage_number),
      ENTITY_SETTER(set_balance_start_voltage_number),
      ENTITY_SETTER(set_balance_stop_diff_voltage_number),
      ENTITY_SETTER(set_nominal_battery_capacity_number),
      ENTITY_SETTER(set_cell_overvoltage_protection_number),
      ENTITY_SETTER(set_cell_overvoltage_recovery_number),
      ENTITY_SETTER(set_cell_undervoltage_protection_number),
      ENTITY_SETTER(set_cell_undervoltage_recovery_number),
      ENTITY_SETTER(set_cell_voltage_difference_protection_number),
      ENTITY_SETTER(set_cell_voltage_difference_recovery_number),
      ENTITY_SETTER(set_cell_shutdown_voltage_number),
      ENTITY_SETTER(set_charging_overcurrent_protection_number),
      ENTITY_SETTER(set_charging_overcurrent_delay_number),
      ENTITY_SETTER(set_charging_overcurrent_recovery_number),
      ENTITY_SETTER(set_discharging_overcurrent_protection_number),
      ENTITY_SETTER(set_discharging_overcurrent_delay_number),
      ENTITY_SETTER(set_discharging_overcurrent_recovery_number),
      ENTITY_SETTER(set_discharging_overcurrent_2_protection_number),
      ENTITY_SETTER(set_discharging_overcurrent_2_delay_number),
      ENTITY_SETTER(set_discharging_overcurrent_2_recovery_number),
      ENTITY_SETTER(set_short_circuit_protection_number),
      ENTITY_SETTER(set_short_circuit_detection_delay_number),
      ENTITY_SETTER(set_short_circuit_recovery_number),
      ENTITY_SETTER(set_charging_overtemperature_protection_number),
      ENTITY_SETTER(set_charging_overtemperature_recovery_number),
      ENTITY_SETTER(set_charging_undertemperature_protection_number),
      ENTITY_SETTER(set_charging_undertemperature_recovery_number),
      ENTITY_SETTER(set_discharging_overtemperature_protection_number),
      ENTITY_SETTER(set_discharging_overtemperature_recovery_number),
      ENTITY_SETTER(set_discharging_undertemperature_protection_number),
      ENTITY_SETTER(set_discharging_undertemperature_recovery_number),
  };
  TestableHeltecBalancerBle bms;
  auto numbers = attach_entities<TestNumber>(bms, setters);

  for (auto &number : numbers) {
    number->publish_state(42.0f);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < numbers.size(); i++) {
    EXPECT_FLOAT_EQ(numbers[i]->state, 42.0f) << setters[i].name;
  }
}

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, KeepsSwitches) {
  const std::vector<NamedSetter<switch_::Switch>> setters = {
      ENTITY_SETTER(set_balancer_switch),
  };
  TestableHeltecBalancerBle bms;
  auto switches = attach_entities<TestSwitch>(bms, setters);

  for (auto &switch_entity : switches) {
    switch_entity->publish_state(true);
  }

  bms.publish_device_unavailable_();

  for (size_t i = 0; i < switches.size(); i++) {
    EXPECT_TRUE(switches[i]->state) << setters[i].name;
  }
}

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, KeepsSelects) {
  const std::vector<NamedSetter<select::Select>> setters = {
      ENTITY_SETTER(set_buzzer_mode_select),
      ENTITY_SETTER(set_battery_type_select),
  };
  TestableHeltecBalancerBle bms;
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

}  // namespace esphome::heltec_balancer_ble::testing
