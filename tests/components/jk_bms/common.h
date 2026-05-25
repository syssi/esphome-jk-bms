#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/jk_bms/jk_bms.h"
#include "esphome/components/switch/switch.h"

namespace esphome::jk_bms::testing {

struct TestSwitch : switch_::Switch {
  void write_state(bool) override {}
};

class TestableJkBms : public JkBms {
 public:
  void update() override {}
};

// Traffic from esp8266-example-faker.yaml
// 265-byte status frame for a 14-cell JK BMS.
// Decoded values (key fields):
//   Cells 1-14: 3.821/3.834/3.831/3.820/3.832/3.834/3.825/3.832/3.811/3.834/3.825/3.835/3.835/3.826 V
//   min_cell_voltage: 3.811 V (cell 9)   max_cell_voltage: 3.835 V (cell 12)
//   delta_cell_voltage: 0.024 V          average_cell_voltage: 3.82821 V
//   total_voltage: 53.59 V               current: 2.08 A (charging, protocol version 1)
//   mosfet_temperature: 29°C         temperature_sensor_1: 30°C  temperature_sensor_2: 28°C
//   state_of_charge: 15 %                total_battery_capacity: 14 Ah
//   errors_bitmask: 0                    operation_modes: charging+discharging+balancer (0x07)
//   cell_count: 14                        charging_cycles: 4
//   software_version: "H6.X__S6.1.3S__" total_runtime: 57856 min ("40d 4h")
//   balancing_switch: on                 charging_switch: on  discharging_switch: on
static const std::vector<uint8_t> STATUS_FRAME_14S = {
    // bytes 0-1: cell section tag + length (14 cells × 3 = 42 bytes)
    0x79,
    0x2A,
    // bytes 2-43: 14 cells (cell_index, volt_hi, volt_lo)
    0x01,
    0x0E,
    0xED,  // cell 1:  3821 → 3.821 V
    0x02,
    0x0E,
    0xFA,  // cell 2:  3834 → 3.834 V
    0x03,
    0x0E,
    0xF7,  // cell 3:  3831 → 3.831 V
    0x04,
    0x0E,
    0xEC,  // cell 4:  3820 → 3.820 V
    0x05,
    0x0E,
    0xF8,  // cell 5:  3832 → 3.832 V
    0x06,
    0x0E,
    0xFA,  // cell 6:  3834 → 3.834 V
    0x07,
    0x0E,
    0xF1,  // cell 7:  3825 → 3.825 V
    0x08,
    0x0E,
    0xF8,  // cell 8:  3832 → 3.832 V
    0x09,
    0x0E,
    0xE3,  // cell 9:  3811 → 3.811 V  (min)
    0x0A,
    0x0E,
    0xFA,  // cell 10: 3834 → 3.834 V
    0x0B,
    0x0E,
    0xF1,  // cell 11: 3825 → 3.825 V
    0x0C,
    0x0E,
    0xFB,  // cell 12: 3835 → 3.835 V  (max)
    0x0D,
    0x0E,
    0xFB,  // cell 13: 3835 → 3.835 V
    0x0E,
    0x0E,
    0xF2,  // cell 14: 3826 → 3.826 V
    // offset = data[1] + 3 = 42 + 3 = 45
    // bytes 44-46: mosfet temperature = 29°C
    0x80,
    0x00,
    0x1D,
    // bytes 47-49: temperature sensor 1 (battery box) = 30°C
    0x81,
    0x00,
    0x1E,
    // bytes 50-52: temperature sensor 2 (battery) = 28°C
    0x82,
    0x00,
    0x1C,
    // bytes 53-55: total voltage = 5359 × 0.01 = 53.59 V
    0x83,
    0x14,
    0xEF,
    // bytes 56-58: current = 0x80D0 → with protocol_version=1: 208 × 0.01 = 2.08 A (charging)
    0x84,
    0x80,
    0xD0,
    // bytes 59-60: capacity remaining = 15 %
    0x85,
    0x0F,
    // bytes 61-62: temperature sensor count = 2
    0x86,
    0x02,
    // bytes 63-65: charging cycles = 4
    0x87,
    0x00,
    0x04,
    // bytes 66-70: total charging cycle capacity = 0 Ah
    0x89,
    0x00,
    0x00,
    0x00,
    0x00,
    // bytes 71-73: cell count = 14
    0x8A,
    0x00,
    0x0E,
    // bytes 74-76: errors bitmask = 0
    0x8B,
    0x00,
    0x00,
    // bytes 77-79: operation mode bitmask = 0x07 (charging + discharging + balancer)
    0x8C,
    0x00,
    0x07,
    // bytes 80-82: total voltage overvoltage protection = 5670 × 0.01 = 56.70 V
    0x8E,
    0x16,
    0x26,
    // bytes 83-85: total voltage undervoltage protection = 4270 × 0.01 = 42.70 V
    0x8F,
    0x10,
    0xAE,
    // bytes 86-88: cell overvoltage protection = 4050 × 0.001 = 4.050 V
    0x90,
    0x0F,
    0xD2,
    // bytes 89-91: cell overvoltage recovery = 4000 × 0.001 = 4.000 V
    0x91,
    0x0F,
    0xA0,
    // bytes 92-94: cell overvoltage protection delay = 5 s
    0x92,
    0x00,
    0x05,
    // bytes 95-97: cell undervoltage protection = 3050 × 0.001 = 3.050 V
    0x93,
    0x0B,
    0xEA,
    // bytes 98-100: cell undervoltage recovery = 3100 × 0.001 = 3.100 V
    0x94,
    0x0C,
    0x1C,
    // bytes 101-103: cell undervoltage protection delay = 5 s
    0x95,
    0x00,
    0x05,
    // bytes 104-106: cell pressure difference protection = 300 × 0.001 = 0.300 V
    0x96,
    0x01,
    0x2C,
    // bytes 107-109: discharging overcurrent protection = 7 A
    0x97,
    0x00,
    0x07,
    // bytes 110-112: discharging overcurrent protection delay = 3 s
    0x98,
    0x00,
    0x03,
    // bytes 113-115: charging overcurrent protection = 5 A
    0x99,
    0x00,
    0x05,
    // bytes 116-118: charging overcurrent protection delay = 5 s
    0x9A,
    0x00,
    0x05,
    // bytes 119-121: balancing start voltage = 3300 × 0.001 = 3.300 V
    0x9B,
    0x0C,
    0xE4,
    // bytes 122-124: balancing delta voltage = 8 × 0.001 = 0.008 V
    0x9C,
    0x00,
    0x08,
    // bytes 125-126: balancing switch = on
    0x9D,
    0x01,
    // bytes 127-129: mosfet overtemperature protection = 90°C
    0x9E,
    0x00,
    0x5A,
    // bytes 130-132: mosfet overtemperature recovery = 70°C
    0x9F,
    0x00,
    0x46,
    // bytes 133-135: battery overtemperature protection = 100°C
    0xA0,
    0x00,
    0x64,
    // bytes 136-138: battery overtemperature recovery = 100°C
    0xA1,
    0x00,
    0x64,
    // bytes 139-141: battery temperature difference protection = 20°C
    0xA2,
    0x00,
    0x14,
    // bytes 142-144: charging overtemperature protection = 70°C
    0xA3,
    0x00,
    0x46,
    // bytes 145-147: discharging overtemperature protection = 70°C
    0xA4,
    0x00,
    0x46,
    // bytes 148-150: charging undertemperature protection = (int16_t)0xFFEC = -20°C
    0xA5,
    0xFF,
    0xEC,
    // bytes 151-153: charging undertemperature recovery = (int16_t)0xFFF6 = -10°C
    0xA6,
    0xFF,
    0xF6,
    // bytes 154-156: discharging undertemperature protection = -20°C
    0xA7,
    0xFF,
    0xEC,
    // bytes 157-159: discharging undertemperature recovery = -10°C
    0xA8,
    0xFF,
    0xF6,
    // bytes 160-161: battery string setting = 14
    0xA9,
    0x0E,
    // bytes 162-166: total battery capacity = 14 Ah
    0xAA,
    0x00,
    0x00,
    0x00,
    0x0E,
    // bytes 167-168: charging MOS switch = on
    0xAB,
    0x01,
    // bytes 169-170: discharging MOS switch = on
    0xAC,
    0x01,
    // bytes 171-173: current calibration = 1041 × 0.001 = 1.041 A
    0xAD,
    0x04,
    0x11,
    // bytes 174-175: device address = 1
    0xAE,
    0x01,
    // bytes 176-177: battery type = 1 → "Ternary Lithium"
    0xAF,
    0x01,
    // bytes 178-180: sleep wait time = 10 s
    0xB0,
    0x00,
    0x0A,
    // bytes 181-182: low_soc_alarm_threshold = 20 %
    0xB1,
    0x14,
    // bytes 183-193: password = "123456" (10 bytes, null-padded)
    0xB2,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x00,
    0x00,
    0x00,
    0x00,
    // bytes 194-195: dedicated charger switch = off
    0xB3,
    0x00,
    // bytes 196-204: device type = "Input Us" (8 bytes)
    0xB4,
    0x49,
    0x6E,
    0x70,
    0x75,
    0x74,
    0x20,
    0x55,
    0x73,
    // bytes 205-209: manufacturing date = "2101"
    0xB5,
    0x32,
    0x31,
    0x30,
    0x31,
    // bytes 210-214: total runtime = 57856 min → sensor: 964.27h, formatted: "40d 4h"
    0xB6,
    0x00,
    0x00,
    0xE2,
    0x00,
    // bytes 215-230: software version = "H6.X__S6.1.3S__" (15 bytes)
    0xB7,
    0x48,
    0x36,
    0x2E,
    0x58,
    0x5F,
    0x5F,
    0x53,
    0x36,
    0x2E,
    0x31,
    0x2E,
    0x33,
    0x53,
    0x5F,
    0x5F,
    // bytes 231-232: start current calibration flag = 0
    0xB8,
    0x00,
    // bytes 233-237: actual battery capacity = 0 Ah
    0xB9,
    0x00,
    0x00,
    0x00,
    0x00,
    // bytes 238-262: manufacturer ID = "BT3072020120000200521001" (24 bytes)
    0xBA,
    0x42,
    0x54,
    0x33,
    0x30,
    0x37,
    0x32,
    0x30,
    0x32,
    0x30,
    0x31,
    0x32,
    0x30,
    0x30,
    0x30,
    0x30,
    0x32,
    0x30,
    0x30,
    0x35,
    0x32,
    0x31,
    0x30,
    0x30,
    0x31,
    // bytes 263-264: protocol version = 1
    0xC0,
    0x01,
};

}  // namespace esphome::jk_bms::testing
